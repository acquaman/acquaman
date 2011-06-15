#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

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

protected:

	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;

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
