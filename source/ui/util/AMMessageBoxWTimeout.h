/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMMESSAGEBOXWTIMEOUT_H
#define AMMESSAGEBOXWTIMEOUT_H

#include <QMessageBox>
#include <QPushButton>

class QTimer;

class AMMessageBoxWTimeout : public QMessageBox
{
Q_OBJECT
public:
	static void showMessageWTimeout(QString title, QString message, QString informativeText="", QString okButtonText="OK", int timeout = 30000);

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
