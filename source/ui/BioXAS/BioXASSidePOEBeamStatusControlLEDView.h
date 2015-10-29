#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H

#include <QWidget>

#include "ui/BioXAS/BioXASSidePOEBeamStatusControlView.h"
#include "ui/beamline/AMControlValueLEDView.h"

class BioXASSidePOEBeamStatusControlLEDView : public BioXASSidePOEBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlLEDView(BioXASSidePOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlLEDView();

public slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view.
	virtual void update();

protected:
	/// The mirror mask LED.
	AMControlValueLEDView *mirrorLED_;
	/// The mono mask LED.
	AMControlValueLEDView *monoLED_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H
