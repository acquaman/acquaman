#include "AMTESTServerConnectionButton.h"

AMTESTServerConnectionButton::AMTESTServerConnectionButton(AMTESTServerConnection *serverConnection, QWidget *parent)
	: QPushButton(parent)
{
	serverConnection_ = serverConnection;
	setText(serverConnection->name());
	setIcon(QIcon(":/10x10/greenLEDOff.png"));

	connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
	connect(serverConnection_, SIGNAL(serverConnecting()), this, SLOT(onConnecting()));
	connect(serverConnection_, SIGNAL(serverConnected()), this, SLOT(onConnected()));
	connect(serverConnection_, SIGNAL(serverError(QString)), this, SLOT(onServerError(QString)));
	connect(serverConnection_, SIGNAL(serverDisconnected()), this, SLOT(onDisconnected()));
}

AMTESTServerConnectionButton::~AMTESTServerConnectionButton()
{

}

void AMTESTServerConnectionButton::onClicked()
{
	if (serverConnection_->connectedToServer())
		serverConnection_->disconnectFromServer();

	else
		serverConnection_->connectToServer();
}

void AMTESTServerConnectionButton::onConnecting()
{
	setIcon(QIcon(":/10x10/yellowLEDOn.png"));
}

void AMTESTServerConnectionButton::onConnected()
{
	setIcon(QIcon(":/10x10/greenLEDOn.png"));
}

void AMTESTServerConnectionButton::onServerError(const QString &errorString)
{
	setIcon(QIcon(":/10x10/redLEDOn.png"));
	setToolTip(errorString);
}

void AMTESTServerConnectionButton::onDisconnected()
{
	setIcon(QIcon(":/10x10/greenLEDOff.png"));
}

