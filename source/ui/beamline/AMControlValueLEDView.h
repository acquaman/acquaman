#ifndef AMCONTROLVALUELEDVIEW_H
#define AMCONTROLVALUELEDVIEW_H

#include "ui/beamline/AMControlLEDView.h"

class AMControlValueLEDView : public AMControlLEDView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlValueLEDView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlValueLEDView();

signals:
	/// Notifier that the green value has changed.
	void greenValueChanged(const AMNumber &newValue);
	/// Notifier that the red value has changed.
	void redValueChanged(const AMNumber &newValue);

public slots:
	/// Updates the view. Reimplemented to respond to the control's value changes.
	virtual void update();

	/// Sets the green value.
	void setGreenValue(const AMNumber &newValue);
	/// Sets the red value.
	void setRedValue(const AMNumber &newValue);
	/// Sets the flag that indicates the LED should be lit and red when the control is valid and not at the green value.
	void setRedWhenNotGreen(bool setRed);

protected:
	/// Returns true if the control is valid, the green value is valid, and the control is at the green value. False otherwise.
	bool atGreenValue() const;
	/// Returns true if the control is valid, the red value is valid, and the control is at the red value. False otherwise.
	bool atRedValue() const;

protected:
	/// The green value.
	AMNumber greenValue_;
	/// The red value.
	AMNumber redValue_;

	/// A flag indicating that the LED should be lit and red when the control is valid and not at the green value.
	bool redWhenNotGreen_;
};

#endif // AMCONTROLVALUELEDVIEW_H
