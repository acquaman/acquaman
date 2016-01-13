#ifndef AMCONTROLCONNECTEDLEDVIEW_H
#define AMCONTROLCONNECTEDLEDVIEW_H

#include "ui/beamline/AMControlLEDView.h"

class AMControlConnectedLEDView : public AMControlLEDView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlConnectedLEDView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlConnectedLEDView();

protected:
	/// Returns true if the LED should currently be unlit and green.
	virtual bool shouldBeGreenOff() const;
	/// Returns true if the LED should currently be lit and green.
	virtual bool shouldBeGreenOn() const;

	/// Returns true if the control is valid and connected, false otherwise.
	virtual bool controlConnected() const;
};

#endif // AMCONTROLCONNECTEDLEDVIEW_H
