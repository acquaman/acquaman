#include "AMTESTStripTool.h"

AMTESTStripTool *AMTESTStripTool::stripTool_ = 0;

AMTESTStripTool *AMTESTStripTool::stripTool()
{
	if (!stripTool_)
		stripTool_ = new AMTESTStripTool;

	return stripTool_;
}

void AMTESTStripTool::releaseStripTool()
{
	if (stripTool_){

		stripTool_->deleteLater();
		stripTool_ = 0;
	}
}

AMTESTServerConnection *AMTESTStripTool::serverConnectionAt(int index) const
{
	if (index >= 0 && index < serverConnections_.size())
		return serverConnections_.at(index);

	return 0;
}

AMTESTServerConnection *AMTESTStripTool::serverConnectionByName(const QString &name) const
{
	foreach (AMTESTServerConnection *serverConnection, serverConnections_)
		if (serverConnection->name() == name)
			return serverConnection;

	return 0;
}

AMTESTStripTool::AMTESTStripTool(QObject *parent)
	: QObject(parent)
{
	setupServerConnections();
}

void AMTESTStripTool::setupServerConnections()
{
	serverConnections_.append(new AMTESTServerConnection("Scaler", AMTESTServerConfiguration("10.52.48.1", 28044), this));
	serverConnections_.append(new AMTESTServerConnection("Ampteks", AMTESTServerConfiguration("10.52.48.40", 28044), this));
	serverConnections_.append(new AMTESTServerConnection("PVs", AMTESTServerConfiguration("10.52.48.31", 28044), this));

	foreach (AMTESTServerConnection *connection, serverConnections_){

		connect(connection, SIGNAL(dataModelsCreated(AMTESTServerConnection*)), this, SIGNAL(dataModelsCreated(AMTESTServerConnection*)));
		connect(connection, SIGNAL(dataModelsDeleted(QStringList)), this, SIGNAL(dataModelsDeleted(QStringList)));
	}
}

