#ifndef AMMESSAGEBOXWTIMEOUT_H
#define AMMESSAGEBOXWTIMEOUT_H

#include <QMessageBox>
#include <QPushButton>

class QTimer;

class AMMessageBoxWTimeout : public QMessageBox
{
Q_OBJECT
public:
 	virtual ~AMMessageBoxWTimeout();
	AMMessageBoxWTimeout(int mSecTimeout = 30000, QWidget *parent = 0);

public slots:
	int execWTimeout();

protected slots:
	void onTimeout();
	void onTimeoutCountdown();

protected:
	virtual void closeEvent(QCloseEvent *event);

protected:
	int mSecTimeout_;
	int timoutCountdown_;
	QTimer *timeoutTimer_;
	QTimer *timeoutCountdownTimer_;
	QPushButton *timeoutCountdownButton_;
};

#endif // AMMESSAGEBOXWTIMEOUT_H
