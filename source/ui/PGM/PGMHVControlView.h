#ifndef PGMHVCONTROLVIEW_H
#define PGMHVCONTROLVIEW_H

#include <QWidget>

#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"
#include "beamline/SXRMB/SXRMBHVControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class QDoubleSpinBox;

class PGMHVControlView : public QWidget
{
    Q_OBJECT
public:
	/// the constructor to display a set of HV controls
	explicit PGMHVControlView(AMControlSet * branchAControlSet, AMControlSet * branchBControlSet,bool viewOnly = false, QWidget *parent = 0);

protected:
	/// the HV control set
	AMControlSet *branchAHVControlSet_;
	AMControlSet *branchBHVControlSet_;

	///
};


class PGMHVControlChannelView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor for the HV Control Channel View
	explicit PGMHVControlChannelView(SXRMBHVControl * hvControlChannel, bool viewOnly = false, QWidget *parent = 0);

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
	void layoutHVControl(SXRMBHVControl *hvControl, bool viewOnly);
	/// to setup the signal/slot connections
	void setupConnections();

protected:
	/// The HV control
	SXRMBHVControl *hvControl_;

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

#endif // PGMHVCONTROLVIEW_H
