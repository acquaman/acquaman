#ifndef AMCONTROLTOOLBUTTON_H
#define AMCONTROLTOOLBUTTON_H

#include "beamline/AMControl.h"
#include "ui/AMToolButton.h"

class AMControlToolButtonColorState
{
public:
	/// Constructor.
	AMControlToolButtonColorState(AMToolButton::ColorState colorState, double controlMinValue, double controlMaxValue) {
		colorState_ = colorState;
		controlMinValue_ = controlMinValue;
		controlMaxValue_ = controlMaxValue;
	}

	/// Alternate constructor. Assumes that the control min and max values are the same, as is the case for most enumerated controls.
	AMControlToolButtonColorState(AMToolButton::ColorState colorState, double controlValue) {
		colorState_ = colorState;
		controlMinValue_ = controlValue;
		controlMaxValue_ = controlValue;
	}

	/// Destructor.
	virtual ~AMControlToolButtonColorState() {}

	/// The color state.
	AMToolButton::ColorState colorState_;
	/// The control min value.
	double controlMinValue_;
	/// The control max value.
	double controlMaxValue_;
};

class AMControlToolButton : public AMToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlToolButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlToolButton();

	/// Returns the control.
	AMControl* control() const { return control_; }
	/// Returns the color states.
	QList<AMControlToolButtonColorState> colorStates() const { return colorStates_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the color states list has changed.
	void colorStatesChanged();

public slots:
	/// Sets the control.
	void setControl(AMControl *newControl);

	/// Adds the given color state to the list of state options, with associated min and max values.
	void addColorState(AMToolButton::ColorState state, double minValue, double maxValue);
	/// Clears the color state options.
	void clearColorStates();

protected slots:
	/// Sets the color states list.
	void setColorStatesList(const QList<AMControlToolButtonColorState> &newList);

	/// Updates the color state.
	virtual void updateColorState();

protected:
	/// Determines the current color state.
	virtual AMToolButton::ColorState getColorState() const;

protected:
	/// The control.
	AMControl *control_;
	/// The list of color states.
	QList<AMControlToolButtonColorState> colorStates_;
};

#endif // AMCONTROLTOOLBUTTON_H
