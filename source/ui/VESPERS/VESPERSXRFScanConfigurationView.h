#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"
#include "ui/VESPERS/XRFSelectionView.h"

#include <QDoubleSpinBox>

class VESPERSXRFScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Default constructor.
	VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent = 0);

	/// Returns a pointer to the current configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

signals:
	/// Passes along the configuration view's start scan signal.
	void startScan();
	/// Signal used on startup to pass on information of already existing regions of interest.
	void roiExistsAlready(AMElement *, QPair<QString, QString>);

protected slots:
	/// Handles new values set from the integration time spin box and passes it along to the control.
	void onIntegrationTimeUpdate();
	/// Handles new values set from the maximum energy spin box and passes it along to the control.
	void onMaximumEnergyUpdate();
	/// Handles the new values from the maximum energy control.
	void onMaximumEnergyControlUpdate(double val);
	/// Handles new values set from the peaking time spin box and passes it along to the control.
	void onPeakingTimeUpdate();
	/// Handles what happens when the start button is clicked.
	void onStartClicked() { detector_->setTime(integrationTime_->value()); emit startScan(); }
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();
	/// Handles what happens when the detector becomes connected.
	void onRoisHaveValues(bool hasValues);

protected:
	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The selection view.
	XRFSelectionView *selectionView_;
	/// The pointer to the detector.
	XRFDetector *detector_;

	/// The integration time spin box.
	QDoubleSpinBox *integrationTime_;
	/// The maximum energy of the detector.  Value is in keV.
	QDoubleSpinBox *maxEnergy_;
	/// The peaking time for the detector.  Value is in us.
	QDoubleSpinBox *peakingTime_;
};

#endif // VESPERSXRFSCANCONFIGURATIONVIEW_H
