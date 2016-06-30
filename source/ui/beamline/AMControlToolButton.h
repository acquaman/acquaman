#ifndef AMCONTROLTOOLBUTTON_H
#define AMCONTROLTOOLBUTTON_H

#include "beamline/AMControl.h"
#include "ui/AMToolButton.h"

Q_DECLARE_METATYPE(QList<AMToolButton::ColorState>)

class AMControlToolButton : public AMToolButton
{
    Q_OBJECT

	Q_PROPERTY(QList<AMToolButton::ColorState> colorStates READ colorStates WRITE setColorStatesList)
	Q_PROPERTY(QList<double> colorStateMinValues READ colorStateMinValues WRITE setColorStateMinValuesList)
	Q_PROPERTY(QList<double> colorStateMaxValues READ colorStateMaxValues WRITE setColorStateMaxValuesList)

public:
	/// Constructor.
	explicit AMControlToolButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlToolButton();

	/// Returns the control.
	AMControl* control() const { return control_; }
	/// Returns the color states.
	QList<AMToolButton::ColorState> colorStates() const { return colorStates_; }
	/// Returns the color state min values.
	QList<double> colorStateMinValues() const { return colorStateMinValues_; }
	/// Returns the color state max values.
	QList<double> colorStateMaxValues() const { return colorStateMaxValues_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Sets the control.
	void setControl(AMControl *newControl);

	/// Adds the given color state to the list of state options, with associated min and max values.
	void addColorState(AMToolButton::ColorState state, double minValue, double maxValue);
	/// Clears the color state options.
	void clearColorStates();

	/// Sets the color states list.
	void setColorStatesList(QList<AMToolButton::ColorState> newStates);
	/// Sets the color state min values list.
	void setColorStateMinValuesList(QList<double> newValues);
	/// Sets the color state max values list.
	void setColorStateMaxValuesList(QList<double> newValues);

protected slots:
	/// Updates the color state.
	virtual void updateColorState();

	/// Testing.
	virtual void mouseReleaseEvent(QMouseEvent *e);

protected:
	/// Determines the current color state.
	virtual AMToolButton::ColorState getColorState() const;

protected:
	/// The control.
	AMControl *control_;

	/// The list of color states.
	QList<AMToolButton::ColorState> colorStates_;
	/// The list of color state min values.
	QList<double> colorStateMinValues_;
	/// The list of color state max values.
	QList<double> colorStateMaxValues_;
};

#endif // AMCONTROLTOOLBUTTON_H
