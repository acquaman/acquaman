#ifndef AMCONTROLVALUELEDVIEW_H
#define AMCONTROLVALUELEDVIEW_H

#include "ui/beamline/AMControlLEDView.h"

class AMControlValueLEDView : public AMControlLEDView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlValueLEDView(AMControl *control, const AMNumber &greenValue = AMNumber::InvalidError, const AMNumber &redValue = AMNumber::InvalidError, QWidget *parent = 0);
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

protected:
	/// The green value.
	AMNumber greenValue_;
	/// The red value.
	AMNumber redValue_;
};

#endif // AMCONTROLVALUELEDVIEW_H
