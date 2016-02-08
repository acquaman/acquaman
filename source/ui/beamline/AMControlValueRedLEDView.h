#ifndef AMCONTROLVALUEREDLEDVIEW_H
#define AMCONTROLVALUEREDLEDVIEW_H

#include "ui/beamline/AMControlLEDView.h"

class AMControlValueRedLEDView : public AMControlLEDView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlValueRedLEDView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlValueRedLEDView();

	/// Returns the red value.
	AMNumber redValue() const { return redValue_; }

signals:
	/// Notifier that the red value has changed.
	void redValueChanged(const AMNumber &newValue);

public slots:
	/// Sets the red value.
	void setRedValue(const AMNumber &newValue);

protected:
	/// Returns true if the LED should currently be red off.
	virtual bool shouldBeRedOff() const;
	/// Returns true if the LED should currently be red on.
	virtual bool shouldBeRedOn() const;

	/// Returns true if the control is valid, the red value is valid, and the control is at the red value. False otherwise.
	virtual bool atRedValue() const;

protected:
	/// The red value.
	AMNumber redValue_;
};

#endif // AMCONTROLVALUEREDLEDVIEW_H
