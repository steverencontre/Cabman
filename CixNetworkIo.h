#ifndef CIXREQUEST_H
#define CIXREQUEST_H

#include <QMainWindow>
#include <QString>
#include <QByteArray>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>

#include <functional>
#include <cassert>

class CixNetworkIo : public QNetworkAccessManager
  {
	Q_OBJECT

  public:
	CixNetworkIo (QObject *parent = nullptr);

	using ReplyHandler = const std::function<void (QJsonDocument)>&;

	void	AuthorisedGet (QString urlstr, ReplyHandler handler);
	void	AuthorisedPost (QString urlstr, const QByteArray& data, ReplyHandler handler);

  signals:
	void		Authorised();

  public slots:
	void		InitSequencer (QNetworkReply *reply);


  private:

	void RequestToken (QString token_endpoint);
	void RequestSync ();

	template <bool POST>
	void	AuthorisedPostOrGet (QString urlstr, const QByteArray*postdata, ReplyHandler handler);

	QJsonDocument		InterpretReply (QNetworkReply *reply);

	enum class State
	{
		IDLE,
		WAIT_OPENID,
		WAIT_AUTH,
		WAIT_SYNC,
		READY
	};

	State				m_State;
	QByteArray	m_Auth;
	QByteArray	m_SyncId;
  };

#endif // CIXREQUEST_H
