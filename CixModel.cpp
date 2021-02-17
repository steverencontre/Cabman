#include "CixModel.h"
#include "Settings.h"


#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>

#include <chrono>
#include <iostream>


using namespace std::literals::chrono_literals;

namespace Cix {


QModelIndex ConfListModel::index (int row, int column, const QModelIndex& parent) const
{
	quintptr id = 0;

	if (!parent.isValid())	// want a top-level item - ie, a conf
	{
		if (row < 0 || row >= m_ConfList.size() || column > 0)
			return {};
	}
	else
	{
		auto pid = parent.internalId();
		auto const& conf = m_ConfList [pid];
		if (row < 0 || row >= conf.GetTopicList().size() || column > 1)
			return {};

		id = (pid + 1) << 24;
	}

	return createIndex (row, column, id | (row << 4) | (column));

}


QModelIndex ConfListModel::parent (const QModelIndex& index) const
{
	auto id = index.internalId();

	auto pid = (id >> 24) - 1;
	if (pid == ~0uLL)
		return {};

	return createIndex (pid, 0, pid << 4);
}


int ConfListModel::rowCount (const QModelIndex& index) const
{
	if (!index.isValid())
		return m_ConfList.size();

	auto id = index.internalId();

	if ((id >> 24) != 0)			// topics have no rows
		return 0;

	return m_ConfList [(id >> 4) & 0xFFFF].GetTopicList().size();
}

int  ConfListModel::columnCount (const QModelIndex& index) const
{
	return !index.isValid() ? 1 : 2;
}



QVariant ConfListModel::data (const QModelIndex& index, int role) const
{
	auto id = index.internalId();

	auto pid = (id >> 24) - 1;
	auto row = (id >> 4) & 0xFFFF;
	auto column = id & 0xF;


	if (pid == ~0ull)
		return {m_ConfList [row].Name()};

	const auto& conf = m_ConfList [pid];
	const auto topic = conf.GetTopicList() [row];
	if (column & 1)
		return {999};			// should be number of messages!

	return {topic.Name()};
}






void Model::run()
{
	setObjectName ("CixModelThread");
	LoadFromDisk();

	// create network IO manager

	m_NetIo = new CixNetworkIo (this);
	connect (m_NetIo, &CixNetworkIo::Authorised, this, [&]() { Poll(); });

	connect (this, SIGNAL (ConfListUpdated()), this, SLOT (onConfListUpdated()));//, Qt::QueuedConnection);

	exec();
}



void Model::LoadFromDisk()
{
	auto default_root = QStandardPaths::standardLocations (QStandardPaths::CacheLocation) [0];
	auto root_dir = Settings{}.getDatabaseRoot (default_root);

	QDir::setCurrent (root_dir);

	QDir dir {"confs"};
	if (!dir.exists())
		return;

	for (auto confname : dir.entryList (QDir::Dirs | QDir::NoDotAndDotDot))
	{
		auto& conf = *m_ConfListModel.m_ConfList.insert (Conf {confname}).first;

		dir.cd (confname);
		for (const auto& topicfile : dir.entryList ({"*.idx"}))
		{
			QString topicname = topicfile.chopped (4);			// remove .idx
			conf.m_TopicList.insert ({topicname});

			// read index for topic...
		}
	}
}




void Model::Poll()
{
	if (!m_GettingSubscriptions)
		GetSubscriptions();

}


void Model::GetSubscriptions()
{
	m_GettingSubscriptions = true;

	m_NetIo->AuthorisedGet ("User/subscriptions/BASIC?sortFlag=NAME",
		[&] (QJsonDocument jdoc)
		{
//			std::cout << jdoc.toJson().toStdString();

			for (auto jobj : jdoc.array())
			{
				auto map {jobj.toObject().toVariantMap()};

				auto confname {map ["name"].toString()};

				m_ConfListModel.m_ConfList.insert ({confname});

//				std::cout << confname.toStdString() << std::endl;
			}

			emit ConfListUpdated();
			m_GettingSubscriptions = false;
		}
	);
}




void Model::onConfListUpdated()
{
	m_TopicsToUpdate = m_ConfListModel.m_ConfList.size();

	for (auto& conf : m_ConfListModel.m_ConfList)
		InitConf (conf);

}


void Model::InitConf (Conf& conf)
{
	auto confname = conf.Name();

	QDir::current().mkpath ("confs/" + confname);
	conf.m_Directory = QDir {"confs/" + confname};

	QString get {"Forum/"};
	get += confname;
	get += "/details?flags=FULL";

	m_NetIo->AuthorisedGet (get,
		[&] (QJsonDocument jdoc)
		{
			auto map {jdoc.toVariant().toMap()};

			auto t1 = map ["topics"];

			auto topics = t1.toMap();

			for (const auto& topic : map ["topics"].toList())
			{
				auto topicinfo = topic.toMap();
				auto topicname = topicinfo ["name"].toString();
				conf.m_TopicList.insert ({topicname});

//				std::cout << topicname.toStdString() << std::endl;
			}

			if (--m_TopicsToUpdate == 0)
				emit ConfTopicsUpdated();

		}
	);


}


} // namespace Cix
