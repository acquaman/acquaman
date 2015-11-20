#ifndef AMTESTSERVERCONNECTIONBUTTON_H
#define AMTESTSERVERCONNECTIONBUTTON_H

#include <QPushButton>

#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

/// This class encapsulates a server connection.  It shows the name and tries to provide some hints to the connectivity of the server.
class AMTESTServerConnectionButton : public QPushButton
{
	Q_OBJECT

public:
	/// Constructor.  Takes a server connection to represent.
	explicit AMTESTServerConnectionButton(AMTESTServerConnection *serverConnection, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTESTServerConnectionButton();

protected slots:
	/// Handles telling the connection to connect or disconnect from the server.
	void onClicked();
	/// Handles when the connecting singal is emitted.
	void onConnecting();
	/// Handles when the connected signal is emitted.
	void onConnected();
	/// Handles when the error signal is emitted.
	void onServerError(const QString &errorString);
	/// Handles when the disconnected singal is emitted.
	void onDisconnected();

	/// Handles popping up the context menu to bring up the configuration edit view.
	void onCustomContextMenuRequested(const QPoint &point);

protected:
	/// Holds the server conection for this button.
	AMTESTServerConnection *serverConnection_;
};

#endif // AMTESTSERVERCONNECTIONBUTTON_H
