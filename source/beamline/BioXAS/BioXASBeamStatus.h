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

	/// Returns true if this control can move, false otherwise. Reimplemented to make all beam status controls read-only by default (for now).
	virtual bool canMove() const { return false; }

	/// Returns true if the beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns true if the beam is off, false otherwise.
	virtual bool isOff() const;

signals:
	/// Notifier that the controls have changed.
	void controlsChanged();

public slots:
	/// Adds a control that helps determine the beam status.
	virtual void addControl(AMControl *newControl, double onValue, double offValue);
	/// Removes a control.
	virtual void removeControl(AMControl *control);
	/// Clears all controls.
	virtual void clearControls();

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASBEAMSTATUS_H
