#ifndef BIOXASAPPCONTROLLER_H
#define BIOXASAPPCONTROLLER_H

#include <QGroupBox>
#include <QWidget>
#include <QLayout>

#include "acquaman/AMScanActionController.h"
#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"
#include "acquaman/BioXAS/BioXASGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#include "application/AMAppControllerSupport.h"
#include "application/CLS/CLSAppController.h"
#include "application/BioXAS/BioXAS.h"

#include "beamline/AMBeamline.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/CLS/CLSStandardsWheel.h"

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

#include "ui/beamline/AMSlitsView.h"

#include "ui/dataman/AMGenericScanEditor.h"

#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSStandardsWheelConfigurationView.h"

#include "ui/BioXAS/BioXAS32ElementGeDetectorView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorView.h"
#include "ui/BioXAS/BioXASXIAFiltersView.h"
#include "ui/BioXAS/BioXASM1MirrorView.h"
#include "ui/BioXAS/BioXASM2MirrorView.h"
#include "ui/BioXAS/BioXASDBHRMirrorsView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerView.h"
#include "ui/BioXAS/BioXASFourElementVortexDetectorView.h"
#include "ui/BioXAS/BioXASEndstationTableView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorView.h"
#include "ui/BioXAS/BioXASValvesView.h"
#include "ui/BioXAS/BioXASFrontEndShuttersView.h"
#include "ui/BioXAS/BioXASFilterFlipperView.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmView.h"
#include "ui/BioXAS/BioXASZebraView.h"
#include "ui/BioXAS/BioXASUtilitiesView.h"
#include "ui/BioXAS/BioXASBeamlineConfigurationView.h"
#include "ui/BioXAS/BioXASSollerSlitView.h"
#include "ui/BioXAS/BioXASCryostatView.h"
#include "ui/BioXAS/BioXASPersistentView.h"

#include "ui/util/AMChooseDataFolderDialog.h"

class BioXASAppController : public CLSAppController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASAppController(const QString &beamlineName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASAppController();

protected slots:
	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();

	/// Handles adding regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	virtual void onRegionOfInterestRemoved(AMRegionOfInterest *region);
	/// Handles updating the regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region);

	/// Sets the beam status view as the current view, with the given control as the selected control.
	void goToBeamlineStatusView(AMControl *control);
	/// Sets the monochromator energy calibration scan configuration view as the current pane.
	void goToEnergyCalibrationScanConfigurationView();
	/// Sets the monochromator energy calibration view as the current pane, and sets the desired scan.
	void goToEnergyCalibrationView(AMScan *toView);

	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	/// Updates the given scan configuration with the given detectors.
	void updateScanConfigurationDetectors(AMGenericStepScanConfiguration *configuration, AMDetectorSet *detectors);
	/// Updates the XAS scan configuration's detectors, using the default detectors provided by BioXASBeamline.
	void updateXASScanConfigurationDetectors();
	/// Updates the generic scan configuration's detectors, using the default detectors provided by BioXASBeamline.
	void updateGenericScanConfigurationDetectors();

protected:
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary DB classes that are beamline-specific.
	virtual void registerDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void registerExporterOptions();
	/// Sets up the available scan configurations.
	virtual void setupScanConfigurations();
	/// Sets up the user configuration.
	virtual void setupUserConfiguration();

	/// The customized implemention for each Beamline to set up the user interface
	virtual void setupUserInterfaceImplementation();
	/// create the persistent view
	virtual void createPersistentView();
	/// create pane for the general controls
	virtual void createGeneralPanes();
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes();
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes();
	/// create pane for the BioXAS components views
	virtual void createComponentsPane();
	/// create pane for the BioXAS calibration views
	virtual void createCalibrationPane();

	/// Creates and returns a view appropriate for viewing the given beamline component. Returns 0 if no view was created.
	virtual QWidget* createComponentView(QObject *component);
	/// Creates and returns a view appropriate for viewing the given scan configuration. Returns 0 if no view was created.
	virtual AMScanConfigurationView* createScanConfigurationView(AMScanConfiguration *configuration);
	/// Creates and returns a view appropriate for viewing the given scan configuration, within a configuration view holder. Returns 0 if no view was created.
	virtual AMScanConfigurationViewHolder3* createScanConfigurationViewWithHolder(AMScanConfiguration *configuration);

	/// Sets up an XAS scan configuration.
	virtual void setupXASScanConfiguration(BioXASXASScanConfiguration *configuration);
	/// Sets up a generic step scan configuration.
	virtual void setupGenericStepScanConfiguration(AMGenericStepScanConfiguration *configuration);

	/// Returns true if the list of regions of interest contains the given ROI.
	bool containsRegionOfInterest(QList<AMRegionOfInterest*> roiList, AMRegionOfInterest *regionOfInterest) const;

protected:
	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	BioXASUserConfiguration *userConfiguration_;

	/// The beam status view.
	CLSBeamlineStatusView *beamlineStatusView_;

	/// The XAS scan configuration.
	BioXASXASScanConfiguration *xasConfiguration_;
	/// The XAS scan configuration view.
	QWidget *xasConfigurationView_;

	/// The generic scan configuration.
	BioXASGenericStepScanConfiguration *genericConfiguration_;
	/// The generic scan configuration view.
	QWidget *genericConfigurationView_;

	/// The mono energy calibration configuration.
	BioXASXASScanConfiguration *energyCalibrationConfiguration_;
	/// The mono energy calibration configuration view.
	QWidget *energyCalibrationConfigurationView_;

	/// The energy calibration view.
	BioXASSSRLMonochromatorEnergyCalibrationView *energyCalibrationView_;

	/// The category name of the components pane
	QString componentPaneCategoryName_;
	/// The component pane icon file.
	QString componentPaneIcon_;
	/// The category name of the calibration pane
	QString calibrationPaneCategoryName_;
	/// The calibration pane icon file.
	QString calibrationPaneIcon_;

};

#endif // BIOXASAPPCONTROLLER_H
