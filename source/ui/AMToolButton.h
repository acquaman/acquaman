#ifndef AMTOOLBUTTON_H
#define AMTOOLBUTTON_H

#include <QToolButton>

class AMToolButton : public QToolButton
{
	Q_OBJECT

	Q_PROPERTY(ColorState colorState READ colorState WRITE setColorState NOTIFY colorStateChanged)
	Q_ENUMS(ColorState)

public:
	/// Enumeration of different button color states.
	enum ColorState { None = 0, Good = 1, Bad = 2, Neutral = 3 };

	/// Constructor.
	AMToolButton(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMToolButton();

	/// Returns the button color state.
	AMToolButton::ColorState colorState() const { return colorState_; }
	/// Returns a string representation of the given color state.
	QString colorStateToString(AMToolButton::ColorState state) const;

signals:
	/// Notifier that the button color state has changed.
	void colorStateChanged(AMToolButton::ColorState newState);

public slots:
	/// Sets the button color state.
	void setColorState(AMToolButton::ColorState newState);

protected:
	/// The button color state.
	ColorState colorState_;
};

#endif // AMTOOLBUTTON_H
