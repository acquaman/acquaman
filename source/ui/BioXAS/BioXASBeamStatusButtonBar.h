#ifndef BIOXASBEAMSTATUSBUTTONBAR_H
#define BIOXASBEAMSTATUSBUTTONBAR_H

#include "ui/BioXAS/BioXASControlButtonBar.h"

class CLSBeamlineStatus;

class BioXASBeamStatusButtonBar : public BioXASControlButtonBar
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASBeamStatusButtonBar(CLSBeamlineStatus *beamlineStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusButtonBar();

	/// Returns the beam status being viewed.
	CLSBeamlineStatus* beamStatus() const { return beamlineStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(CLSBeamlineStatus *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status.
	void setBeamlineStatus(CLSBeamlineStatus *newControl);

protected:
	/// Creates and returns a button for the given control.
	virtual QAbstractButton* createButton(AMControl *control, double greenValue);

protected:
	/// The beam status being viewed.
	CLSBeamlineStatus *beamlineStatus_;
};

#endif // BIOXASBEAMSTATUSBUTTONBAR_H
