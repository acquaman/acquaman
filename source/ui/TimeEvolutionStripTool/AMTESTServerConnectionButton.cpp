#include "AMTESTServerConnectionButton.h"

#include <QMenu>
#include <QAction>
#include <QInputDialog>

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

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
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

void AMTESTServerConnectionButton::onCustomContextMenuRequested(const QPoint &point)
{
	QMenu popup(this);

	popup.addAction("Customize Server Configuration")->setDisabled(true);
	popup.addSeparator();
	popup.addAction("Change hostname");
	popup.addAction("Change port number");

	QAction *temp = popup.exec(mapToGlobal(point));

	// If a valid action was selected.
	if (temp){
		if (temp->text() == "Change hostname"){

			QString newHostName = QInputDialog::getText(this, "New Host Name", "Host Name", QLineEdit::Normal, serverConnection_->hostName());
			serverConnection_->setHostName(newHostName);
		}

		else if (temp->text() == "Change port number"){

			quint16 newPortNumber = quint16(QInputDialog::getInt(this, "New Port Number", "Port Number", serverConnection_->portNumber(), 0));
			serverConnection_->setPortNumber(newPortNumber);
		}
	}
}

