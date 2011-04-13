#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "ui/VESPERS/XRFDetectorView.h"

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
	/// Passes along the detector view's start scan signal.
	void startScan();
	/// Passes along the detector view's stop scan signal.
	void stopScan();

protected slots:
	/// Handles new values set from the integration time spin box and passes it along to the control.
	void onIntegrationTimeUpdate();

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
