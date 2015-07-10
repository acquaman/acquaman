#ifndef AMGOOGLEAUTHTOKENREQUEST_H
#define AMGOOGLEAUTHTOKENREQUEST_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QTcpSocket>

/*!
  * \brief A class which represents the procedure to obtain an access token from
  * Google's OAuthv2 system.
  *
  * This process requires the current user of the system to log in to their google
  * account part way through, in order to grant the access requested by the system.
  * As such it is necessary to listen for the authorizationPageToDisplay() function
  * and render the passed HTML in a webview.
  * The class will handle everything else, eventually emitting a tokenReceived
  * signal which contains the token string, the time (in seconds) till this token
  * expires, and a refresh token. The refresh token should be stored long term,
  * as it can be used in future to obtain access without having to go through
  * the authorization procedure a second time.
  */
class AMGoogleAuthTokenRequest : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an authorization token request which will obtain
	  * a token using the provided information.
	  * \param clientId ~ The client id obtained from the Google Developer Console
	  * \param clientSecret ~ The client secret obtained from the Google Developer
	  * Console.
	  * \param redirectPort ~ The port to listen on for Goggle's responses.
	  * \param scope ~ The scope which indicates what account information the token
	  * should grant access to.
	  * \param endPoint ~ The url of google's oauth2 server. Defaults to
	  *  https://accounts.google.com/o/oauth2/auth
	  */
	explicit AMGoogleAuthTokenRequest(const QString& clientId,
									  const QString& clientSecret,
									  int redirectPort,
									  const QString& scope,
									  const QString& endpoint = "https://accounts.google.com/o/oauth2/auth",
									  QObject *parent = 0);

	/*!
	  * Frees the resources owned by this authorization token request.
	  */
	virtual ~AMGoogleAuthTokenRequest();

	/*!
	  * Begins the process of obtaining the authorization token using the information
	  * the request was initialized with.
	  */
	void sendRequest();
signals:
	/*!
	  * Signal indicating that the process needs to request authorization from
	  * the user to obtain the access token.
	  * \param html ~ The HTML code of the page which is used to redirect the user
	  * to the authorization page.
	  */
	void authorizationPageToDisplay(const QString& html);

	/*!
	  * Signal indicating that the token request process has been completed.
	  * \param token ~ The access token granted by google.
	  * \param secsToExpiration ~ The number of seconds till the token expires.
	  * \param refreshToken ~ The token which can be used to obtain a new access
	  * token when the expiration time elapses.
	  */
	void tokenReceived(const QString& token,
					   int secsToExpiration,
					   const QString& refreshToken);

protected slots:
	void onNetworkReplyReceived(QNetworkReply* reply);
	void onNewCodeServerConnection();
	void onCodeReadyToRead();
protected:

	QString endPoint_;
	QString clientId_;
	QString clientSecret_;
	int redirectPort_;
	QString scope_;
	QString code_;
	QNetworkAccessManager* manager_;
	QTcpServer* codeListenServer_;
	QTcpSocket* codeListenSocket_;

};

#endif // AMGOOGLEAUTHTOKENREQUEST_H
