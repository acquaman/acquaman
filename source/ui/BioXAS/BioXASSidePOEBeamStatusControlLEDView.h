#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSidePOEBeamStatusControl.h"
#include "ui/beamline/AMControlValueLEDView.h"

class BioXASSidePOEBeamStatusControlLEDView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlLEDView(BioXASSidePOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlLEDView();

	/// Returns the beam status control being viewed.
	BioXASSidePOEBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view, clears and updates it.
	void refresh();

	/// Sets the beam status control being viewed.
	void setControl(BioXASSidePOEBeamStatusControl *newControl);

protected:
	/// The beam status control being viewed.
	BioXASSidePOEBeamStatusControl *control_;

	/// The front-end beam status LED.
	AMControlValueLEDView *frontEndLED_;
	/// The mirror mask LED.
	AMControlValueLEDView *mirrorLED_;
	/// The mono mask LED.
	AMControlValueLEDView *monoLED_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLLEDVIEW_H
