#ifndef CLSAPPCONTROLLER_H
#define CLSAPPCONTROLLER_H

#include "application/AMAppController.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

class CLSAppController : public AMAppController
{
    Q_OBJECT

public:
	explicit CLSAppController(const QString &beamlineName, QObject *parent = 0);
	virtual ~CLSAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();
	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected:
	virtual AMFacility facility() const	{ return clsFacility_; }

	/// Initializes the periodic table.
	virtual void initializePeriodicTable();
	/// Initializes the storage ring object.
	virtual void initializeStorageRing();

	/// Initializes the beamline object.
	virtual void initializeBeamline() = 0;
	/// Registers all of the necessary DB classes that are beamline-specific.
	virtual void registerClasses() = 0;
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions() = 0;
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface() = 0;
	/// Sets up all of the connections.
	virtual void makeConnections() = 0;

protected:
	/// the definition of the current facility
	AMFacility clsFacility_;
};

#endif // CLSAPPCONTROLLER_H
