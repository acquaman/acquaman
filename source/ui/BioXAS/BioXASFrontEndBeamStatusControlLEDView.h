#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H

#include <QWidget>

#include "ui/BioXAS/BioXASFrontEndBeamStatusControlView.h"
#include "ui/beamline/AMControlValueLEDView.h"

class BioXASFrontEndBeamStatusControlLEDView : public BioXASFrontEndBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlLEDView(BioXASFrontEndBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlLEDView();

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the latest beam status control information.
	void update();

protected:
	/// The shutters LED.
	AMControlValueLEDView *shuttersLED_;
	/// The valves LED.
	AMControlValueLEDView *valvesLED_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H
