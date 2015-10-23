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

	/// Returns the green value.
	AMNumber greenValue() const { return greenValue_; }

signals:
	/// Notifier that the green value has changed.
	void greenValueChanged(const AMNumber &newValue);

public slots:
	/// Sets the green value.
	void setGreenValue(const AMNumber &newValue);

protected:
	/// Returns true if the LED should currently be green.
	virtual bool shouldBeGreenOn() const;
	/// Returns true if the LED should currently be green.
	virtual bool shouldBeGreenOff() const;

	/// Returns true if the LED should currently be red.
	virtual bool shouldBeRedOn() const;

	/// Returns true if the control is valid, the green value is valid, and the control is at the green value. False otherwise.
	bool atGreenValue() const;

protected:
	/// The green value.
	AMNumber greenValue_;
};

#endif // AMCONTROLVALUELEDVIEW_H
