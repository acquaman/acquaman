#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSidePOEBeamStatusControl.h"

class BioXASSidePOEBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlView();

	/// Returns the beam status control being viewed.
	BioXASSidePOEBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	virtual void clear() = 0;
	/// Updates the view.
	virtual void update() = 0;
	/// Refreshes the view, clears and updates it.
	virtual void refresh();

	/// Sets the beam status control being viewed.
	virtual void setControl(BioXASSidePOEBeamStatusControl *newControl);

protected:
	/// The beam status control being viewed.
	BioXASSidePOEBeamStatusControl *control_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLVIEW_H
