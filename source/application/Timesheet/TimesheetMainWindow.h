#ifndef TIMESHEETMAINWINDOW_H
#define TIMESHEETMAINWINDOW_H

#include <QMainWindow>
#include <QWebView>

class AMGoogleAuthTokenRequest;
class TimesheetMainWindow : public QMainWindow
{
    Q_OBJECT
public:
	explicit TimesheetMainWindow(QWidget *parent = 0);

	virtual ~TimesheetMainWindow();
signals:

public slots:

protected slots:
	void onAuthorizationPageToDisplay(const QString& html);
	void onTokenReceived(const QString& token,
						 int secsToExpiration,
						 const QString& refreshToken);

protected:
	QWebView* authorizationWebView_;
	AMGoogleAuthTokenRequest* tokenRequestHandler_;
};

#endif // TIMESHEETMAINWINDOW_H
