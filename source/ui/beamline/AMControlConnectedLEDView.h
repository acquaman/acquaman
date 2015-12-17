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

public slots:
	/// Updates the view. Reimplemented from AMControlLEDView to respond to the control's connected state.
	virtual void update();
};

#endif // AMCONTROLCONNECTEDLEDVIEW_H
