/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef CLSPROCSERVMANAGER_H
#define CLSPROCSERVMANAGER_H

#include <QObject>
#include <QTimer>

#include "qttelnet/qttelnet.h"

class CLSProcServManager : public QObject
{
	Q_OBJECT
public:
	CLSProcServManager(const QString &hostname, quint16 port, const QString &description, QObject *parent = 0);

	QString description() const { return description_;}

public slots:
	void logInToService(){
		telnet_ = new QtTelnet(this);
		telnet_->setSocket(new QTcpSocket(this));
		connect(telnet_, SIGNAL(message(QString)), this, SLOT(onLoginReply(QString)));
		telnet_->connectToHost(hostname_, port_);
	}

	void restartService(){
		connect(telnet_, SIGNAL(message(QString)), this, SLOT(onShutDownReply(QString)));

		QString myHex = "18";
		telnet_->sendData(QString(QByteArray::fromHex(myHex.toAscii())));
	}

signals:
	void loggedIn();
	void restarted();

protected slots:
	void onLoginReply(const QString &msg){
		qDebug() << "LOGIN REPLY";
		loginReplyStrings_.append(msg);
		if(!startedLoginReply_){
			startedLoginReply_ = true;
			QTimer::singleShot(1000, this, SLOT(disconnectLoginReply()));
		}
	}

	void disconnectLoginReply(){
		qDebug() << "Disconnecting login \n" << loginReplyStrings_;
		disconnect(telnet_, SIGNAL(message(QString)), this, SLOT(onLoginReply(QString)));
		emit loggedIn();
	}

	void onShutDownReply(const QString &msg){
		qDebug() << "SHUTDOWN REPLY";
		shutDownReplyStrings_.append(msg);
		if(!startedShutDownReply_){
			startedShutDownReply_ = true;
			QTimer::singleShot(1000, this, SLOT(disconnectShutDownReply()));
		}
	}

	void disconnectShutDownReply(){
		qDebug() << "Disconnecting shutdown \n" << shutDownReplyStrings_;
		disconnect(telnet_, SIGNAL(message(QString)), this, SLOT(onShutDownReply(QString)));
		connect(telnet_, SIGNAL(message(QString)), this, SLOT(onRestartReply(QString)));

		QString myHex = "12";
		telnet_->sendData(QString(QByteArray::fromHex(myHex.toAscii())));
	}

	void onRestartReply(const QString &msg){
		qDebug() << "RESTART REPLY";
		restartReplyStrings_.append(msg);
		if(!startedRestartReply_){
			startedRestartReply_ = true;
			QTimer::singleShot(1000, this, SLOT(disconnectRestartReply()));
		}
	}

	void disconnectRestartReply(){
		qDebug() << "Disconnecting restart \n" << restartReplyStrings_;
		disconnect(telnet_, SIGNAL(message(QString)), this, SLOT(onRestartReply(QString)));
		connect(telnet_, SIGNAL(message(QString)), this, SLOT(collectAdditionalMessages(QString)));
		emit restarted();

		QTimer::singleShot(5000, this, SLOT(startServerDisconnect()));
	}

	void startServerDisconnect(){
		qDebug() << "Starting telnet disconnect";
		//telnet_->sendData("quit");
		telnet_->socket()->close();

		delete telnet_;
		telnet_ = 0;
	}

	void collectAdditionalMessages(const QString &msg){
		qDebug() << msg;
	}

protected:
	QString stripCR(const QString &msg)
	{
		QString nmsg(msg);
		nmsg.remove('\r');
		nmsg.remove(QRegExp("\033\\[[0-9;]*[A-Za-z]")); // Also remove terminal control codes
		return nmsg;
	}

protected:
	QtTelnet *telnet_;

	QString hostname_;
	quint16 port_;
	QString description_;

	bool startedLoginReply_;
	bool startedShutDownReply_;
	bool startedRestartReply_;

	QString loginReplyStrings_;
	QString shutDownReplyStrings_;
	QString restartReplyStrings_;
};

#include <QWidget>
#include <QFormLayout>

class CLSProcServManagerView : public QWidget
{
Q_OBJECT
public:
	CLSProcServManagerView(QList<CLSProcServManager*> procServs, QWidget *parent = 0);

protected:
	QList<CLSProcServManager*> procServs_;

	QFormLayout *fl_;
};

#endif // CLSPROCSERVMANAGER_H
