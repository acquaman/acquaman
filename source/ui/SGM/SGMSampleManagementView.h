#ifndef SGMSAMPLEMANAGEMENTVIEW_H
#define SGMSAMPLEMANAGEMENTVIEW_H

#include "ui/beamline/AMBeamlineSampleManagementView.h"

class QDoubleSpinBox;

class AMAction3;

class SGMSampleManagementView : public AMBeamlineSampleManagementView
{
Q_OBJECT
public:
	SGMSampleManagementView(QWidget *parent = 0);

	~SGMSampleManagementView();

protected slots:
	/// Handles moving to the transfer position
	void onTransferPositionButtonClicked();
	/// Handles moving to the measurement position
	void onMeasurePositionButtonClicked();

	/// Handles turning the lights off (0)
	void onLightsOffButtonClicked();
	/// Handles turning the lights on (100)
	void onLightsOnButtonClicked();
	/// Handles changing values to arbitrary value
	void onLightLevelSpinBoxValueChanged(double value);
	/// Handles updating the spin box to reflect the control value
	void onLightLevelControlValueChanged(double value);

protected:
	/// Button for transfer position
	QPushButton *transferPositionButton_;
	/// Button for measurement position
	QPushButton *measurePositionButton_;
	/// Action list for transfer position
	AMAction3 *transferPositionActions_;
	/// Action list for measurement position
	AMAction3 *measurementPositionActions_;

	/// Button for lights off
	QPushButton *lightsOffButton_;
	/// Button for lights on
	QPushButton *lightsOnButton_;
	/// Spin box for light level as percent
	QDoubleSpinBox *lightLevelSpinBox_;
};

#endif // SGMSAMPLEMANAGEMENTVIEW_H
