#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "beamline/BioXAS/BioXASSideBeamStatusControl.h"

class BioXASBeamStatus : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatus();

	/// Returns true if the beam status is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatusControl() const { return frontEndBeamStatus_; }
	/// Returns the Side beam status control.
	BioXASSideBeamStatusControl* sideBeamStatusControl() const { return sideBeamStatus_; }

protected:
	/// The front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatus_;
	/// The Side beam status control.
	BioXASSideBeamStatusControl *sideBeamStatus_;
};

#endif // BIOXASBEAMSTATUS_H
