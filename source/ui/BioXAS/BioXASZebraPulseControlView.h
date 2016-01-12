#ifndef BIOXASZEBRAPULSECONTROLVIEW_H
#define BIOXASZEBRAPULSECONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASZebraPulseControl.h"

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>

/// An pulse control view.
class BioXASZebraPulseControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraPulseControlView(BioXASZebraPulseControl *pulseControl, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Sets the input value.
	void setInputValue();
	/// Sets the delay before value.
	void setDelayBeforeValue();
	/// Sets the pulse width value.
	void setPulseWidthValue();
	/// Handles the input status light.
	void onInputStatusChanged(bool status);
	/// Handles updating the edge radio buttons.
	void onEdgeTriggerValueChanged(int value);
	/// Handles setting the pulse control when the edge trigger changes.
	void onEdgeTriggerButtonsChanged();
	/// Handles updating the delay before spin box.
	void onDelayBeforeValueChanged();
	/// Handles updating the pulse width spin box.
	void onPulseWidthValueChanged();
	/// Handles setting the time units when the pulse control changes.
	void onTimeUnitsValueChanged(int value);
	/// Handles the trigger while active light.
	void onTriggerWhileActiveChanged(bool status);
	/// handles the output status light.
	void onOutputStatusChanged(bool status);

protected:
	/// The pulse control model.
	BioXASZebraPulseControl *pulseControl_;

	/// The input control.
	QSpinBox *inputSpinBox_;
	/// The input control label.
	QLabel *inputLabel_;
	/// The input status light.
	QLabel *inputStatusLight_;
	/// The rising edge button.
	QRadioButton *risingEdgeButton_;
	/// The falling edge button.
	QRadioButton *fallingEdgeButton_;
	/// The delay before spin box.
	QDoubleSpinBox *delayBeforeSpinBox_;
	/// The pulse width spin box.
	QDoubleSpinBox *pulseWidthSpinBox_;
	/// The button group for the time units.
	QButtonGroup *timeUnitsGroup_;
	/// The trigger while active light.
	QLabel *triggerWhileActiveLight_;
	/// The output status light.
	QLabel *outputStatusLight_;
};

#endif // BIOXASZEBRAPULSECONTROLVIEW_H
