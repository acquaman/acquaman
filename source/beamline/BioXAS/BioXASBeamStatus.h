#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASBeamStatus : public BioXASBiStateGroup
{
    Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { On = 0, Off = 1 };

	/// Constructor.
	explicit BioXASBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatus();

	/// Returns true if the beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns true if the beam is off, false otherwise.
	virtual bool isOff() const { return !isOn(); }

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

//	/// Creates and returns a new move action to 'On'.
//	virtual AMAction3* createMoveToOnAction();
//	/// Creates and returns a new move action to 'Off'.
//	virtual AMAction3* createMoveToOffAction();

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASBEAMSTATUS_H
