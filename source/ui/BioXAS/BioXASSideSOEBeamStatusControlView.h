#ifndef BIOXASSIDESOEBEAMSTATUSCONTROLVIEW_H
#define BIOXASSIDESOEBEAMSTATUSCONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSideSOEBeamStatusControl.h"

class BioXASSideSOEBeamStatusControlView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatusControlView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatusControlView();

	/// Returns the beam status control being viewed.
	BioXASSideSOEBeamStatusControl* control() const { return control_; }

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
	void setControl(BioXASSideSOEBeamStatusControl *newControl);

protected:
	/// The beam status control being viewed.
	BioXASSideSOEBeamStatusControl *control_;
};

#endif // BIOXASSIDESOEBEAMSTATUSCONTROLVIEW_H
