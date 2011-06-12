#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"
#include "ui/VESPERS/CustomizeRegionsOfInterest.h"

#include <QDoubleSpinBox>
#include <QLabel>

class VESPERSXRFScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Default constructor.
	VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent = 0);
	/// Destructor.
	~VESPERSXRFScanConfigurationView();

	/// Returns a pointer to the current configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

signals:
	/// Passes along the configuration view's start scan signal.
	void startScan();

protected slots:
	/// Shows/Hides the more advanced settings in the detector.
	void onAdvancedSettingsChanged(bool advanced);
	/// Handles new values set from the integration time spin box and passes it along to the control.
	void onIntegrationTimeUpdate();
	/// Handles new values set from the minimum energy spin box and passes it along to the contorl.
	void onMinimumEnergyUpdate();
	/// Handles new values set from the maximum energy spin box and passes it along to the control.
	void onMaximumEnergyUpdate();
	/// Handles the new values from the maximum energy control.
	void onMaximumEnergyControlUpdate(double val);
	/// Handles new values set from the peaking time spin box and passes it along to the control.
	void onPeakingTimeUpdate();
	/// Handles what happens when the start button is clicked.
	void onStartClicked() { detector_->setTime(integrationTime_->value()); emit startScan();; }
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();

protected:

	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;
	/// Pointer to the XRFPeriodicTable inside configuration.
	XRFPeriodicTable *xrfTable_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The pointer to the detector.
	XRFDetector *detector_;
	/// The pointer to the customize view.
	CustomizeRegionsOfInterest *customize_;

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

#endif // VESPERSXRFSCANCONFIGURATIONVIEW_H
