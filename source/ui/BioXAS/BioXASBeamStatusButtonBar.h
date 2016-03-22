#ifndef BIOXASBEAMSTATUSBUTTONBAR_H
#define BIOXASBEAMSTATUSBUTTONBAR_H

#include "ui/BioXAS/BioXASControlButtonBar.h"

class BioXASBeamStatus;

class BioXASBeamStatusButtonBar : public BioXASControlButtonBar
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASBeamStatusButtonBar(BioXASBeamStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusButtonBar();

	/// Returns the beam status being viewed.
	BioXASBeamStatus* beamStatus() const { return beamStatus_; }

signals:
	/// Notifier that the beam status being viewed has changed.
	void beamStatusChanged(BioXASBeamStatus *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the beam status.
	void setBeamStatus(BioXASBeamStatus *newControl);

protected:
	/// Creates and returns a button for the given control.
	virtual QAbstractButton* createButton(AMControl *control, double greenValue);

protected:
	/// The beam status being viewed.
	BioXASBeamStatus *beamStatus_;
};

#endif // BIOXASBEAMSTATUSBUTTONBAR_H
