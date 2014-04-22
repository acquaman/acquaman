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


#include "CLSProcServManager.h"

//#include <QPushButton>

//CLSProcServManager::CLSProcServManager(const QString &hostname, quint16 port, const QString &description, QObject *parent) :
//	QObject(parent)
//{
//	hostname_ = hostname;
//	port_ = port;
//	description_ = description;

//	startedLoginReply_ = false;
//	startedShutDownReply_ = false;
//	startedRestartReply_ = false;

//	telnet_ = 0; //NULL
//	//telnet_ = new QtTelnet(this);
//	//connect(telnet_, SIGNAL(message(QString)), this, SLOT(onLoginReply(QString)));
//	//telnet_->connectToHost(hostname, port);
//}

//CLSProcServManager::~CLSProcServManager(){}

//CLSProcServManagerView::CLSProcServManagerView(QList<CLSProcServManager *> procServs, QWidget *parent) :
//	QWidget(parent)
//{
//	procServs_ = procServs;

//	fl_ = new QFormLayout();

//	QPushButton *tempButton;
//	for(int x = 0; x < procServs_.count(); x++){
//		tempButton = new QPushButton("Restart");
//		fl_->addRow(procServs_.at(x)->description(), tempButton);
//		//connect(tempButton, SIGNAL(clicked()), procServs_.at(x), SLOT(restartService()));
//		connect(tempButton, SIGNAL(clicked()), procServs_.at(x), SLOT(logInToService()));
//		connect(procServs_.at(x), SIGNAL(loggedIn()), procServs_.at(x), SLOT(restartService()));
//	}

//	setLayout(fl_);
//}
//CLSProcServManagerView::~CLSProcServManagerView(){}
