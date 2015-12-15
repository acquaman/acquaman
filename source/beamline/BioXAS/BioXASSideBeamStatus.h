#ifndef BIOXASSIDEBEAMSTATUS_H
#define BIOXASSIDEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineBeamStatus.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASSideSOEBeamStatus.h"

class BioXASSideBeamStatus : public BioXASBeamlineBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatus(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatus();

	/// Returns the POE beam status.
	BioXASSidePOEBeamStatus* poeBeamStatus() const { return poeBeamStatus_; }
	/// Returns the SOE beam status.
	BioXASSideSOEBeamStatus* soeBeamStatus() const { return soeBeamStatus_; }

signals:
	/// Notifier that the POE beam status control has changed.
	void poeBeamStatusChanged(BioXASSidePOEBeamStatus *newControl);
	/// Notifier that the SOE beam status control has changed.
	void soeBeamStatusChanged(BioXASSideSOEBeamStatus *newControl);

public slots:
	/// Sets the POE beam status control.
	void setPOEBeamStatus(BioXASSidePOEBeamStatus *newControl);
	/// Sets the SOE beam status control.
	void setSOEBeamStatus(BioXASSideSOEBeamStatus *newControl);

protected slots:
	/// Updates the connected state. Reimplemented to consider particular children.
	virtual void updateConnected();

protected:
	/// The POE beam status control.
	BioXASSidePOEBeamStatus *poeBeamStatus_;
	/// The SOE beam status control.
	BioXASSideSOEBeamStatus *soeBeamStatus_;
};

#endif // BIOXASSIDEBEAMSTATUS_H
