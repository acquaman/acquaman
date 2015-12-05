#ifndef BIOXASFRONTENDBEAMSTATUS_H
#define BIOXASFRONTENDBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"

class BioXASFrontEndValves;

class BioXASFrontEndBeamStatus : public BioXASBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASFrontEndBeamStatus(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatus();

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const { return !isOn(); }
};

#endif // BIOXASFRONTENDBEAMSTATUS_H
