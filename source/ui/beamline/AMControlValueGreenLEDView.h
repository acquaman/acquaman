#ifndef AMCONTROLVALUEGREENLEDVIEW_H
#define AMCONTROLVALUEGREENLEDVIEW_H

#include "ui/beamline/AMControlLEDView.h"
#include "dataman/AMNumber.h"

class AMControlValueGreenLEDView : public AMControlLEDView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlValueGreenLEDView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlValueGreenLEDView();

	/// Returns the green value.
	AMNumber greenValue() const { return greenValue_; }

signals:
	/// Notifier that the green value has changed.
	void greenValueChanged(const AMNumber &newValue);

public slots:
	/// Sets the green value.
	void setGreenValue(const AMNumber &newValue);

protected:
	/// Returns true if the LED should currently be green off.
	virtual bool shouldBeGreenOff() const;
	/// Returns true if the LED should currently be green on.
	virtual bool shouldBeGreenOn() const;

	/// Returns true if the control is valid, the green value is valid, and the control is at the green value. False otherwise.
	virtual bool atGreenValue() const;

protected:
	/// The green value.
	AMNumber greenValue_;
};

#endif // AMCONTROLVALUEGREENLEDVIEW_H
