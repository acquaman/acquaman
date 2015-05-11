#ifndef SXRMBSCANCONFIGURATIONVIEW_H
#define SXRMBSCANCONFIGURATIONVIEW_H

#include "application/SXRMB/SXRMB.h"
#include "acquaman/SXRMB/SXRMBScanConfiguration.h"

#include "ui/acquaman/AMScanConfigurationView.h"

class QComboBox;
class QCheckBox;
class QGroupBox;

class SXRMBScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor takes an SXRMB scan configuration to view
	SXRMBScanConfigurationView(QWidget *parent = 0);
	/// Destructor
	virtual ~SXRMBScanConfigurationView();

protected slots:
	/// Slot that updates the fluorescence detector buttons.
	void updateFluorescenceDetectorComboBox(SXRMB::FluorescenceDetectors detector);
	/// Handles changes to detector selection
	virtual void onFluorescenceDetectorChanged(int detector) = 0;
	/// Handles changes to power on TEY HV control flag
	virtual void onPowerOnTEYHVControlEnabled(bool) = 0;

protected:
	/// helper function to create the comboBox for bruker detector
	QComboBox *createFluorescenceComboBox();
	/// helper function to create and layout detector settins
	QGroupBox *createAndLayoutDetectorSettings(SXRMBScanConfiguration * configuration);
	/// helper function to update the poweron HV control checkbox text
	void updatePowerOnHVControlCheckBoxText();

protected:
	/// Checkbox for enabling the TEY HV control automatically
	QCheckBox *powerOnHVControlCheckBox_;
	/// ComboBox for choosing Bruker detector
	QComboBox *fluorescenceDetectorComboBox_;

};

#endif // SXRMBSCANCONFIGURATIONVIEW_H
