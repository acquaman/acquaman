#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"

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

protected slots:
	/// Handles new values set from the integration time spin box and passes it along to the control.
	void onIntegrationTimeUpdate();
	/// Handles what happens when the start button is clicked.
	void onStartClicked() { detector_->setTime(integrationTime_->value()); emit startScan(); }
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();

protected:
	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The pointer to the detector.
	XRFDetector *detector_;

	/// The integration time spin box.
	QDoubleSpinBox *integrationTime_;
};

#endif // VESPERSXRFSCANCONFIGURATIONVIEW_H
