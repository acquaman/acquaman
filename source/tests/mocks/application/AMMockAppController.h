#ifndef AMMOCKAPPCONTROLLER_H
#define AMMOCKAPPCONTROLLER_H

#include "application/AMAppController.h"

class AMMockAppController : public AMAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call AMMockAppController::start() to create the object.
	explicit AMMockAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~AMMockAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:

protected:

	// Things to do on startup.
	/// Registers all of the necessary mock classes.
	void registerClasses();
	/// Check and set up the database for the first time run.
	void setupOnFirstRun();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

protected:

};

#endif // AMMOCKAPPCONTROLLER_H
