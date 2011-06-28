#ifndef XRFFREERUNVIEW_H
#define XRFFREERUNVIEW_H

#include <QWidget>
#include <QToolButton>

#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"
#include "ui/VESPERS/CustomizeRegionsOfInterest.h"
#include "ui/VESPERS/VESPERSXRFScanConfigurationView.h"
#include "acquaman/VESPERS/XRFFreeRun.h"
#include "ui/AMWorkflowManagerView.h"
#include "beamline/AMBeamlineScanAction.h"

class XRFFreeRunView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit XRFFreeRunView(XRFFreeRun *xrfFreeRun, AMWorkflowManagerView *workflow, QWidget *parent = 0);

protected slots:
	/// Handles what happens when the start button is clicked.
	void onStartClicked() { start_->setDisabled(true); detector_->setTime(integrationTime_->value()); workflow_->insertBeamlineAction(-1, new AMBeamlineScanAction(xrfFreeRun_->configuration()->createCopy()), true); }
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();
	/// Changes the disabled state of the start button based on the status of the detector.
	void onStatusChanged(bool status) { start_->setDisabled(status); }
	/// Shows/Hides the more advanced settings in the detector.
	void onAdvancedSettingsChanged(bool advanced);
	/// Handles new values set from the integration time spin box and passes it along to the configuration.
	void onIntegrationTimeUpdate() { detector_->setTime(integrationTime_->value()); }
	/// Handles new values set from the minimum energy spin box and passes it along to the configuration.
	void onMinimumEnergyUpdate() { xrfTable_->setMinimumEnergy(minEnergy_->value()*1000); }
	/// Handles new values set from the maximum energy spin box and passes it along to the configuration.
	void onMaximumEnergyUpdate() { detector_->setMaximumEnergyControl(maxEnergy_->value()); xrfTable_->setMaximumEnergy(maxEnergy_->value()*1000); }
	/// Handles new values for the maximum energy from the detector.
	void onMaximumEnergyChanged(double val) { xrfTable_->setMaximumEnergy(val*1000); }
	/// Handles new values set from the peaking time spin box and passes it along to the configuration.
	void onPeakingTimeUpdate() { detector_->setPeakingTimeControl(peakingTime_->value()); }

protected:

	/// Pointer to the XRFPeriodicTable inside configuration.
	XRFPeriodicTable *xrfTable_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The pointer to the detector.
	XRFDetector *detector_;
	/// The pointer to the customize view.
	CustomizeRegionsOfInterest *customize_;
	/// The pointer to the xrf free run model.
	XRFFreeRun *xrfFreeRun_;
	/// The pointer to the workflow.neh
	AMWorkflowManagerView *workflow_;

	/// The start button.
	QToolButton *start_;
	/// The integration time spin box.
	QDoubleSpinBox *integrationTime_;
	/// The label for the minimum energy spin box.
	QLabel *minEnergyLabel_;
	/// The minimum energy of the detector.  Value is in keV.
	QDoubleSpinBox *minEnergy_;
	/// The label for the maximum energy spin box.
	QLabel *maxEnergyLabel_;
	/// The maximum energy of the detector.  Value is in keV.
	QDoubleSpinBox *maxEnergy_;
	/// The label for the peaking time spin box.
	QLabel *peakingTimeLabel_;
	/// The peaking time for the detector.  Value is in us.
	QDoubleSpinBox *peakingTime_;
};

#endif // XRFFREERUNVIEW_H
