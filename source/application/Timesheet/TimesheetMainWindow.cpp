#include "TimesheetMainWindow.h"
#include <QDebug>
#include "AMGoogleAuthTokenRequest.h"

TimesheetMainWindow::TimesheetMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	tokenRequestHandler_ =
			new AMGoogleAuthTokenRequest("??CLIENT_ID??",
										 "??CLIENT_SECRET??",
										 53204,
										 "https://www.googleapis.com/auth/calendar");

	connect(tokenRequestHandler_, SIGNAL(authorizationPageToDisplay(QString)),
			this, SLOT(onAuthorizationPageToDisplay(QString)));

	connect(tokenRequestHandler_, SIGNAL(tokenReceived(QString,int,QString)),
			this, SLOT(onTokenReceived(QString,int,QString)));

	authorizationWebView_ = new QWebView();

	tokenRequestHandler_->sendRequest();
}

TimesheetMainWindow::~TimesheetMainWindow()
{
	tokenRequestHandler_->deleteLater();
	authorizationWebView_->setVisible(false);
	authorizationWebView_->deleteLater();
}

void TimesheetMainWindow::onAuthorizationPageToDisplay(const QString &html)
{
	authorizationWebView_->setHtml(html);
	authorizationWebView_->setVisible(true);
}

#include <QDebug>
void TimesheetMainWindow::onTokenReceived(const QString &token, int secsToExpiration, const QString &refreshToken)
{
	authorizationWebView_->setVisible(false);
	qDebug() << "Token: " << token
			 << "\nExpires in: " << secsToExpiration
			 << " s\nRefresh token: " << refreshToken;
}
