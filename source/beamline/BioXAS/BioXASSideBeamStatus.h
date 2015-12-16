#ifndef BIOXASSIDEBEAMSTATUS_H
#define BIOXASSIDEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASSideSOEBeamStatus.h"

class BioXASSideBeamStatus : public BioXASSideSOEBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatus();

	/// Returns the front-end beam status.
	BioXASFrontEndBeamStatus* frontEndStatus() const { return frontEndStatus_; }
	/// Returns the POE beam status.
	BioXASSidePOEBeamStatus* poeStatus() const { return poeStatus_; }
	/// Returns the SOE beam status.
	BioXASSideSOEBeamStatus* soeStatus() const { return soeStatus_; }

signals:
	/// Notifier that the front-end beam status has changed.
	void frontEndStatusChanged(BioXASFrontEndBeamStatus *newStatus);
	/// Notifier that the POE beam status has changed.
	void poeStatusChanged(BioXASSidePOEBeamStatus *newStatus);
	/// Notifier that the SOE beam status has changed.
	void soeStatusChanged(BioXASSideSOEBeamStatus *newStatus);

public slots:
	/// Sets the front-end shutters control.
	virtual void setShutters(BioXASFrontEndShutters *newControl);
	/// Sets the valves control.
	virtual void setValves(BioXASMasterValves *newControl);
	/// Sets the pre-mirror mask state control.
	virtual void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask control.
	virtual void setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl);
	/// Sets the endstation shutter control.
	virtual void setEndstationShutter(CLSBiStateControl *newControl);

	/// Sets the front-end beam status.
	void setFrontEndStatus(BioXASFrontEndBeamStatus *newStatus);
	/// Sets the POE beam status.
	void setPOEStatus(BioXASSidePOEBeamStatus *newStatus);
	/// Sets the SOE beam status.
	void setSOEStatus(BioXASSideSOEBeamStatus *newStatus);

protected slots:
	/// Updates the connected state. Reimplemented to consider particular children.
	virtual void updateConnected();

protected:
	/// The front-end beam status control.
	BioXASFrontEndBeamStatus *frontEndStatus_;
	/// The POE beam status control.
	BioXASSidePOEBeamStatus *poeStatus_;
	/// The SOE beam status control.
	BioXASSideSOEBeamStatus *soeStatus_;
};

#endif // BIOXASSIDEBEAMSTATUS_H
