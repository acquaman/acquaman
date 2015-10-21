#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "ui/beamline/AMControlValueLEDView.h"

class BioXASFrontEndBeamStatusControlLEDView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlLEDView(BioXASFrontEndBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlLEDView();

	/// Returns the front end beam status control being viewed.
	BioXASFrontEndBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the front end beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the latest beam status control information.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the beam status control being viewed.
	void setControl(BioXASFrontEndBeamStatusControl *newControl);

protected:
	/// The beam status control being viewed.
	BioXASFrontEndBeamStatusControl *control_;

	/// The shutters LED.
	AMControlValueLEDView *shuttersLED_;
	/// The valves LED.
	AMControlValueLEDView *valvesLED_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLLEDVIEW_H
