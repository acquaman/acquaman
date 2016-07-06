#ifndef AMCONTROLTOOLBUTTON_H
#define AMCONTROLTOOLBUTTON_H

#include "beamline/AMControl.h"
#include "ui/AMToolButton.h"

typedef QList<AMToolButton::ColorState> AMToolButtonColorStateList;

class AMControlToolButton : public AMToolButton
{
    Q_OBJECT

	Q_PROPERTY(AMToolButtonColorStateList colorStates READ colorStates WRITE setColorStatesList NOTIFY colorStatesChanged)
	Q_PROPERTY(AMDoubleList colorStateMinValues READ colorStateMinValues WRITE setColorStateMinValuesList NOTIFY colorStateMinValuesChanged)
	Q_PROPERTY(AMDoubleList colorStateMaxValues READ colorStateMaxValues WRITE setColorStateMaxValuesList NOTIFY colorStateMaxValuesChanged)

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
	AMDoubleList colorStateMinValues() const { return colorStateMinValues_; }
	/// Returns the color state max values.
	AMDoubleList colorStateMaxValues() const { return colorStateMaxValues_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the color states list has changed.
	void colorStatesChanged();
	/// Notifier that the color state min values list has changed.
	void colorStateMinValuesChanged();
	/// Notifier that the color state max values list has changed.
	void colorStateMaxValuesChanged();

public slots:
	/// Sets the control.
	void setControl(AMControl *newControl);

	/// Adds the given color state to the list of state options, with associated min and max values.
	void addColorState(AMToolButton::ColorState state, double minValue, double maxValue);
	/// Clears the color state options.
	void clearColorStates();

protected slots:
	/// Sets the color states list.
	void setColorStatesList(const AMToolButtonColorStateList &newStates);
	/// Sets the color state min values list.
	void setColorStateMinValuesList(const AMDoubleList &newValues);
	/// Sets the color state max values list.
	void setColorStateMaxValuesList(const AMDoubleList &newValues);

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

Q_DECLARE_METATYPE(AMToolButton::ColorState);
Q_DECLARE_METATYPE(AMToolButtonColorStateList);

#endif // AMCONTROLTOOLBUTTON_H
