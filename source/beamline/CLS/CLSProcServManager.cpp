#include "CLSProcServManager.h"

CLSProcServManager::CLSProcServManager(const QString &hostname, quint16 port, const QString &description, QObject *parent) :
	QObject(parent)
{
	hostname_ = hostname;
	port_ = port;
	description_ = description;

	startedLoginReply_ = false;
	startedShutDownReply_ = false;
	startedRestartReply_ = false;

	telnet_ = 0; //NULL
	//telnet_ = new QtTelnet(this);
	//connect(telnet_, SIGNAL(message(QString)), this, SLOT(onLoginReply(QString)));
	//telnet_->connectToHost(hostname, port);
}

#include <QPushButton>
CLSProcServManagerView::CLSProcServManagerView(QList<CLSProcServManager *> procServs, QWidget *parent) :
	QWidget(parent)
{
	procServs_ = procServs;

	fl_ = new QFormLayout();

	QPushButton *tempButton;
	for(int x = 0; x < procServs_.count(); x++){
		tempButton = new QPushButton("Restart");
		fl_->addRow(procServs_.at(x)->description(), tempButton);
		//connect(tempButton, SIGNAL(clicked()), procServs_.at(x), SLOT(restartService()));
		connect(tempButton, SIGNAL(clicked()), procServs_.at(x), SLOT(logInToService()));
		connect(procServs_.at(x), SIGNAL(loggedIn()), procServs_.at(x), SLOT(restartService()));
	}

	setLayout(fl_);
}
