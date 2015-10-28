#ifndef BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H
#define BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"

class BioXASFrontEndBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControlView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControlView();

	/// Returns the beam status control being viewed.
	BioXASFrontEndBeamStatusControl* control() const { return control_; }

signals:
	/// Notifier that the beam status control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	virtual void clear() = 0;
	/// Updates the view with the latest beam status control information.
	virtual void update() = 0;
	/// Refreshes the view.
	virtual void refresh() = 0;

	/// Sets the beam status control being viewed.
	virtual void setControl(BioXASFrontEndBeamStatusControl *newControl);

protected:
	/// The beam status being viewed.
	BioXASFrontEndBeamStatusControl *control_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROLVIEW_H
