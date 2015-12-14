#ifndef BIOXASBEAMLINEBEAMSTATUS_H
#define BIOXASBEAMLINEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"

class BioXASBeamlineBeamStatus : public BioXASBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineBeamStatus();

	/// Returns the front-end beam status.
	BioXASFrontEndBeamStatus* frontEndBeamStatus() const { return frontEndStatus_; }

signals:
	/// Notifier that the front-end beam status control has changed.
	void frontEndBeamStatusChanged(BioXASFrontEndBeamStatus *newControl);

public slots:
	/// Sets the front-end beam status control.
	void setFrontEndBeamStatus(BioXASFrontEndBeamStatus *newControl);

protected:
	/// The front-end beam status control.
	BioXASFrontEndBeamStatus *frontEndStatus_;
};

#endif // BIOXASBEAMLINEBEAMSTATUS_H
