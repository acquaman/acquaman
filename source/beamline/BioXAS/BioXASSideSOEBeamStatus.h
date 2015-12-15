#ifndef BIOXASSIDESOEBEAMSTATUS_H
#define BIOXASSIDESOEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineBeamStatus.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASSideSOEBeamStatus : public BioXASBeamlineBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideSOEBeamStatus(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatus();

	/// Returns the POE beam status control.
	BioXASSidePOEBeamStatus* poeBeamStatus() const { return poeBeamStatus_; }
	/// Returns the endstation shutter control.
	CLSBiStateControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the POE beam status control has changed.
	void poeBeamStatusChanged(BioXASSidePOEBeamStatus *newControl);
	/// Notifier that the endstation shutter control has changed.
	void endstationShutterChanged(CLSBiStateControl *newControl);

public slots:
	/// Sets the POE beam status control.
	void setPOEBeamStatus(BioXASSidePOEBeamStatus *newControl);
	/// Sets the endstation shutter control.
	void setEndstationShutter(CLSBiStateControl *newControl);

protected slots:
	/// Updates the connected state. Reimplemented to consider particular children.
	virtual void updateConnected();

protected:
	/// The POE beam status control.
	BioXASSidePOEBeamStatus *poeBeamStatus_;
	/// The endstation shutter control.
	CLSBiStateControl *endstationShutter_;
};

#endif // BIOXASSIDESOEBEAMSTATUS_H
