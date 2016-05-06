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
	virtual AMFacility facility() const { return clsFacility_; }

	/// Sets up local and remote data paths.
	virtual bool setupDataFolder() = 0;

	/// Initializes the periodic table.
	virtual void initializePeriodicTable();
	/// Initializes the storage ring object.
	virtual void initializeStorageRing();

	/// Initializes the beamline object.
	virtual void initializeBeamline() = 0;
	/// Registers all of the necessary DB classes that are beamline-specific.
	virtual void registerDBClasses() = 0;
	/// Sets up all of the exporter options for the various scan types.
	virtual void registerExporterOptions() = 0;
	/// Sets up the available scan configurations.
	virtual void setupScanConfigurations() = 0;
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up all of the connections.
	virtual void makeConnections() = 0;
	/// Sets up the user configuration.
	virtual void setupUserConfiguration() = 0;

	/// The customized implemention for each Beamline to set up the user interface
	virtual void setupUserInterfaceImplementation() {}
	/// create the persistent view
	virtual void createPersistentView() {}
	/// create pane for the general controls
	virtual void createGeneralPanes()  {}
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes()  {}
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes()  {}

protected:
	/// the definition of the current facility
	AMFacility clsFacility_;

	/// The general pane category name
	QString generalPaneCategeryName_;
	/// The detector pane category name
	QString detectorPaneCategoryName_;
	/// The scan pane category name
	QString scanPaneCategoryName_;

	/// The general pane icon file.
	QString generalPaneIcon_;
	/// The detectors pane icon file.
	QString detectorsPaneIcon_;
	/// The scans pane icon file.
	QString scansPaneIcon_;
};

#endif // CLSAPPCONTROLLER_H
