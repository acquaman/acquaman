#ifndef AMCONTROLTOOLBUTTON_H
#define AMCONTROLTOOLBUTTON_H

#include "beamline/AMControl.h"
#include "ui/AMToolButton.h"

class AMControlToolButton : public AMToolButton
{
    Q_OBJECT

	Q_PROPERTY(QList<AMToolButton::ColorState> colorStates READ colorStates WRITE setColorStatesList NOTIFY colorStatesChanged)
	Q_PROPERTY(QList<QVariant> colorStateMinValues READ colorStateMinValues WRITE setColorStateMinValuesList NOTIFY colorStateMinValuesChanged)
	Q_PROPERTY(QList<QVariant> colorStateMaxValues READ colorStateMaxValues WRITE setColorStateMaxValuesList NOTIFY colorStateMaxValuesChanged)

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
	QList<QVariant> colorStateMinValues() const { return colorStateMinValues_; }
	/// Returns the color state max values.
	QList<QVariant> colorStateMaxValues() const { return colorStateMaxValues_; }

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
	void setColorStatesList(const QList<AMToolButton::ColorState> &newStates);
	/// Sets the color state min values list.
	void setColorStateMinValuesList(const QList<QVariant> &newValues);
	/// Sets the color state max values list.
	void setColorStateMaxValuesList(const QList<QVariant> &newValues);

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
	QList<QVariant> colorStateMinValues_;
	/// The list of color state max values.
	QList<QVariant> colorStateMaxValues_;
};

Q_DECLARE_METATYPE(AMToolButton::ColorState)
Q_DECLARE_METATYPE(QList<AMToolButton::ColorState>)

#endif // AMCONTROLTOOLBUTTON_H
