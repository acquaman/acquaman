#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"

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

protected:
	/// The front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatus_;
};

#endif // BIOXASBEAMSTATUS_H
