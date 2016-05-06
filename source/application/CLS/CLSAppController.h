#ifndef CLSAPPCONTROLLER_H
#define CLSAPPCONTROLLER_H

#include "application/AMAppController.h"
#include "dataman/AMRun.h"

#include "util/AMPeriodicTable.h"

#define CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD 30101

class CLSAppController : public AMAppController
{
    Q_OBJECT

public:
	explicit CLSAppController(const QString &beamlineName, QObject *parent = 0);
	virtual ~CLSAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

protected slots:
	/// Helper slot that connects generic scan editors that use the 2D scan view to the app controller so that it can enable quick configuration of scans.
	void onScanEditorCreated(AMGenericScanEditor *editor);
	/// implementation for slot that connects generic scan editors that use the 2D scan view to the app controller so that it can enable quick configuration of scans.
	virtual void onScanEditorCreatedImplementation(AMGenericScanEditor *editor);

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
	/// Sets up the user configuration.
	virtual void setupUserConfiguration() = 0;

	/// The customized implemention for each Beamline to set up the user interface
	virtual void setupUserInterfaceImplementation();
	/// create the persistent view
	virtual void createPersistentView() = 0;
	/// create pane for the general controls
	virtual void createGeneralPanes() = 0;
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes() = 0;
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes() = 0;

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
