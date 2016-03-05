#ifndef AMTESTAPPCONTROLLER_H
#define AMTESTAPPCONTROLLER_H

#include <QObject>

#define AMTESTAPPCONTROLER_STARTING 324000
#define AMTESTAPPCONTROLER_STARTED 324001
#define AMTESTAPPCONTROLER_SHUTTING_DOWN 324002
#define AMTESTAPPCONTROLER_SHUTDOWN 324003
#define AMTESTAPPCONTROLER_DATASERVER_CONNECTED 324004
#define AMTESTAPPCONTROLER_REGISTERING_CLASSES 324005
#define AMTESTAPPCONTROLER_INITIALIZING_DATASERVER 324006
#define AMTESTAPPCONTROLER_SETTING_UP_GUI 324007

class AMTESTMainWindow;

/// Runs the time evolution strip tool.
class AMTESTAppController : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	AMTESTAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTAppController();

	/// Gets everything setup for the app to run.
	bool startup();
	/// Cleans everything up to make sure the program can be exited successfully.
	bool shutdown();
	/// Getter for whether the app controller is running.  It is running if it was successful during startup().
	bool isRunning();

protected slots:
	/// Handles any initialization actions that are required after the data server is connected.
	void onClientControllerConnected();

protected:
	/// Handles the registration of all necessary classes.
	void registerClasses();
	/// Initializes the client app controller.
	void initializeDataServer();
	/// Sets up the widgets used in the UI.
	void setupUI();

	/// Holds whether the program was successfully started and is considered running.
	bool isRunning_;
	/// The pointer to the main window widget.
	AMTESTMainWindow *mainWindow_;
};

#endif // AMTESTAPPCONTROLLER_H
