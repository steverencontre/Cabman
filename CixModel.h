#ifndef CIXMODEL_H
#define CIXMODEL_H



#include <QTimer>
#include <QThread>
#include <QString>
#include <QDir>
#include <QAbstractItemModel>

#include <ctime>
#include <vector>
#include <map>
#include <functional>

namespace ranges = std::ranges;

#include "CixNetworkIo.h"

namespace Cix {

class Conf;
class Topic;
class Message;
class User;


template <typename T>
class SortedVectorMap : private std::vector<T>
{
public:
	using vector = std::vector<T>;
	using typename vector::iterator;
	using vector::begin;
	using vector::end;
	using vector::size;
	using vector::empty;
	using vector::operator[];

	SortedVectorMap() = default;
	SortedVectorMap (vector&& v) : vector {std::move (v)} { ranges::sort (*this); }

	std::pair<iterator, bool> insert (T&& conf)
	{
//		auto it = ranges::lower_bound (*this, conf);
		auto it = std::lower_bound (begin(), end(), conf);
		if (it != end() && it->Name() == conf.Name())
			return {it, false};

		return {vector::insert (it, std::move (conf)), true};
	}

//	const T& operator[] (QString name) const	{ return *ranges::lower_bound (*this, name); }

};


using ConfList = SortedVectorMap<Conf>;
using TopicList = SortedVectorMap<Topic>;

using MessageList = std::map<int, Message>;
using UserList = std::map<QString, User>;

class ConfListModel : public QAbstractItemModel
{
public:

	QModelIndex	index				(int row, int column, const QModelIndex& parent={}) const override;
	QModelIndex	parent				(const QModelIndex&) const override;
	int					rowCount			(const QModelIndex& parent={}) const override;
	int					columnCount	(const QModelIndex& parent={}) const override;
	QVariant		data					(const QModelIndex& index, int role=Qt::DisplayRole) const override;

private:
	friend class Model;
	ConfList		m_ConfList;
};


class Model : public QThread
{
	Q_OBJECT

public:
	Model (QObject *parent=nullptr) : QThread {parent}{}
	~Model() override	{}

	void run() override;


	ConfListModel *GetConfListModel() { return &m_ConfListModel; }

signals:
	void			ConfListUpdated();
	void			ConfTopicsUpdated();

private slots:
	void			onConfListUpdated();

private:

	void			LoadFromDisk();
	void			Poll();

	void			GetSubscriptions();

	void			InitConf (Conf& conf);


	CixNetworkIo	  *m_NetIo {nullptr};
	bool					m_GettingSubscriptions {false};
	QDir					m_DbRoot;

	QTimer				m_PollTimer;

	ConfListModel		m_ConfListModel;
	int						m_TopicsToUpdate {0};

};


class Message
{
public:
	Message() {}

	size_t			Number() const;
	time_t			DateTime() const;
	const User&	Author() const;

private:
	friend class Model;
};


class NamedEntity
{
public:
	NamedEntity (QString name) : m_Name {name} {}

	QString	Name() const { return m_Name; }

	bool operator< (const NamedEntity& e)	{ return m_Name < e.m_Name; }
	bool operator< (QString name)				{ return m_Name < name; }

protected:
	QString			m_Name;
};



class Topic : public NamedEntity
{
	Topic (QString name) : NamedEntity {name} {}

	bool		Joined() const;
	const		MessageList& GetMessageList() const { return m_MessageList; }

private:
	friend class Model;

	bool				m_Subscribed;
	MessageList	m_MessageList;

};


class Conf : public NamedEntity
{
public:
	Conf (QString name) : NamedEntity {name} {}

	bool		Joined() const;
	const		TopicList& GetTopicList() const { return m_TopicList; }

private:
	friend class Model;

	bool			m_Subscribed;
	QDir			m_Directory;
	TopicList	m_TopicList;
};


} // namespace Cix

#endif // CIXMODEL_H
