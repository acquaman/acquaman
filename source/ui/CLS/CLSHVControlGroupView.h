#ifndef CLSHVCONTROLGROUPVIEW_H
#define CLSHVCONTROLGROUPVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSHVControl.h"

class CLSHVControlGroupView : public QWidget
{
	Q_OBJECT
public:
	/// the constructor to display two sets of HV controls
	explicit CLSHVControlGroupView(AMControlSet * hvControlSetA, AMControlSet * hvControlSetB, bool viewOnly = false, QWidget *parent = 0);

	/// the constructor to display one sets of HV controls
	explicit CLSHVControlGroupView(AMControlSet * hvControlSet, bool viewOnly = false, QWidget *parent = 0);

protected:
	/// helper function to create views for HV controls of a given set
	QWidget * layoutHVControls(AMControlSet *hvControlSet, bool viewOnly = false);

protected:
	/// the HV control set
	AMControlSet *hvControlSetA_;
	AMControlSet *hvControlSetB_;
};


class CLSHVControlChannelView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor for the HV Control Channel View
	explicit CLSHVControlChannelView(CLSHVControl * hvControlChannel, bool viewOnly = false, QWidget *parent = 0);

protected slots:
	/// to handle the HVControlStatusChanged signal
	void onHVControlStatusChanged(int status);
	/// to handle the HVControlVoltageValueChanged signal
	void onHVControlVoltageValueChanged(double value);
	/// to handle the HVControlCurrentValueChanged signal
	void onHVControlMeasuredCurrentValueChanged(double value);

	/// to handle the Voltage SpinBox value changed signal
	void onVoltageSpinBoxEditFinished();

protected:
	/// to layout the High Voltage control ui components
	void layoutHVControl(CLSHVControl *hvControl, bool viewOnly);
	/// to setup the signal/slot connections
	void setupConnections();

protected:
	/// The HV control
	CLSHVControl *hvControl_;

	/// The layout to hold the ui components
	QHBoxLayout * contentLayout_;
	/// the label to display the HV channel name
	QLabel *nameLabel_;
	/// the label to display the Voltage value, which is only visible for viewOnly
	QLabel *actVoltageLabel_;
	/// the spinbox to display / edit the Voltage value, which is only visible for edit view
	QDoubleSpinBox *voltageSpinBox_;
	/// the button to toggle power on/off for the HV control
	QPushButton *powerOnOffButton_;
	/// the label to display the current
	QLabel *measuredCurrentLabel_;
	/// the label to display the status
	QLabel *statusLabel_;
};


#endif // CLSHVCONTROLGROUPVIEW_H
