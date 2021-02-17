#include "CixNetworkIo.h"


#include <QUuid>

#include "Settings.h"

#include <QUrlQuery>
#include <iostream>



#define CIXBASE			"https://api.cix.uk/"
#define API3BASE			CIXBASE "v3.0/"
#define CLIENT_ID			"cabman@rsn-tech.co.uk"
#define SECRET				"$y$j9T$78FdUcESp07se/AwbtpCh03."


constexpr char CREDENTIAL[]		{CLIENT_ID ":" SECRET};
constexpr char URL_OPENID[]		{CIXBASE ".well-known/openid-configuration"};




CixNetworkIo::CixNetworkIo (QObject *parent) : QNetworkAccessManager {parent}
  {
	Settings settings;
	m_SyncId = settings.getCixSyncDeltaId();
	{
		m_SyncId = QUuid::createUuid().toByteArray (QUuid::Id128);
		settings.setCixSyncDeltaId (m_SyncId);
	}

	connect (this, &QNetworkAccessManager::finished, this, &CixNetworkIo::InitSequencer);

	get (QNetworkRequest (QUrl (URL_OPENID)));
	m_State = State::WAIT_OPENID;
}




QJsonDocument CixNetworkIo::InterpretReply (QNetworkReply *reply)
{
	int e = reply->error();
	if (e)
		std::cout << " err " << e;

	reply->deleteLater();

	QByteArray buffer = reply->readAll();
	return QJsonDocument::fromJson (buffer);
}



void CixNetworkIo::InitSequencer (QNetworkReply *reply)
{
	auto jdoc {InterpretReply (reply)};

	switch (m_State)
	{
	case State::WAIT_OPENID:
		RequestToken (jdoc ["token_endpoint"].toString());
		break;

	case State::WAIT_AUTH:
		m_Auth = QByteArray {"Bearer "} + jdoc ["access_token"].toString().toLocal8Bit();

//		RequestSync();						// sync doesn't seem to work: go straight to READY and do incremental fill
		m_State = State::READY;

		emit Authorised();

		break;

//	case State::WAIT_SYNC:
//		m_State = State::READY;
//		emit Ready();

	default:
		;
	}

	if (m_State == State::READY)
		disconnect (this, &QNetworkAccessManager::finished, this, &CixNetworkIo::InitSequencer);
}




void CixNetworkIo::RequestToken (QString token_endpoint)
{
	QNetworkRequest request {QUrl (token_endpoint)};

	request.setHeader (QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader ("Authorization", QByteArray {"basic "} + QByteArray {CREDENTIAL}.toBase64());

	QUrlQuery params;

	params.addQueryItem("grant_type", "password");
	params.addQueryItem("username", "steveren");
	params.addQueryItem("password", "malta");
	params.addQueryItem("scope", "cixApi3");

	post (request, params.query (QUrl::FullyEncoded).toUtf8());
	m_State = State::WAIT_AUTH;
}


void CixNetworkIo::RequestSync()
{
	static const QByteArray base {"User/"};
	static const QByteArray sync {"/sync"};

	AuthorisedGet (base + m_SyncId + sync, [] (QJsonDocument) {});		// connect reply to dummy handler, InitSequencer will take care of this
	m_State = State::WAIT_SYNC;

}

template <bool POST>
void	CixNetworkIo::AuthorisedPostOrGet (QString urlstr, const QByteArray*postdata, ReplyHandler handler)
{
	assert (m_State == State::READY);

	QNetworkRequest request {QUrl (API3BASE + urlstr)};
	request.setRawHeader ("Authorization", m_Auth);

	QNetworkReply *reply;
	if constexpr (POST)
		reply = post (request, *postdata);
	else
		reply = get (request);

	connect (reply, &QNetworkReply::finished, [=,this]() { handler (InterpretReply (reply)); });
}


void	CixNetworkIo::AuthorisedGet (QString urlstr, ReplyHandler handler)
{
	AuthorisedPostOrGet<false> (urlstr, nullptr, handler);
}


void	CixNetworkIo::AuthorisedPost (QString urlstr, const QByteArray& data, ReplyHandler handler)
{
	AuthorisedPostOrGet<true> (urlstr, &data, handler);
}
