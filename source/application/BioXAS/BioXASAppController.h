#ifndef BIOXASAPPCONTROLLER_H
#define BIOXASAPPCONTROLLER_H

#include <QGroupBox>
#include <QWidget>
#include <QLayout>

#include "acquaman/AMScanActionController.h"
#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "acquaman/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"

#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "application/BioXAS/BioXAS.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSFacilityID.h"

#include "dataman/AMRun.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "dataman/BioXAS/BioXASUserConfiguration.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"

#include "ui/AMMainWindow.h"
#include "ui/AMTopFrame.h"
#include "ui/AMMotorGroupView.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/BioXAS/BioXAS32ElementGeDetectorView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"
#include "ui/BioXAS/BioXASXIAFiltersView.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmView.h"
#include "ui/BioXAS/BioXASM1MirrorView.h"
#include "ui/BioXAS/BioXASM2MirrorView.h"
#include "ui/BioXAS/BioXASDBHRMirrorView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerView.h"
#include "ui/BioXAS/BioXASFourElementVortexDetectorView.h"
#include "ui/BioXAS/BioXASEndstationTableView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"
#include "ui/CLS/CLSJJSlitsView.h"
#include "ui/CLS/CLSStandardsWheelConfigurationView.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "util/AMPeriodicTable.h"

class BioXASAppController : public AMAppController
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();
	/// Destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();

	/// Handles adding regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	virtual void onRegionOfInterestRemoved(AMRegionOfInterest *region);

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	/// Registers all of the necessary classes that are BioXAS-specific.
	virtual void registerClasses();

	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions();
	/// Initializes the storage ring object.
	virtual void initializeStorageRing();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Initializes the periodic table.
	virtual void initializePeriodicTable();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder() { return false; }

protected:
	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	BioXASUserConfiguration *userConfiguration_;
};

#endif // BIOXASAPPCONTROLLER_H
