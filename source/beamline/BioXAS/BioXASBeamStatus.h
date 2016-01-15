#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASShutters;
class BioXASValves;
class BioXASM1MirrorMaskState;
class BioXASSSRLMonochromatorMaskState;
class CLSBiStateControl;

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

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the shutters.
	BioXASShutters* shutters() const { return shutters_; }
	/// Returns the valves.
	BioXASValves* valves() const { return valves_; }
	/// Returns the pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState* mirrorMaskState() const { return mirrorMaskState_; }
	/// Returns the pre-mono mask state control.
	BioXASSSRLMonochromatorMaskState* monoMaskState() const { return monoMaskState_; }

signals:
	/// Notifier that the controls have changed.
	void controlsChanged();
	/// Notifier that the shutters have changed.
	void shuttersChanged(BioXASShutters *newShutters);
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASValves *newValves);
	/// Notifier that the pre-mirror mask state control has changed.
	void mirrorMaskStateChanged(BioXASM1MirrorMaskState *newControl);
	/// Notifier that the pre-mono mask state control has changed.
	void monoMaskStateChanged(BioXASSSRLMonochromatorMaskState *newControl);

public slots:
	/// Sets the shutters control.
	void setShutters(BioXASShutters *newControl);
	/// Sets the valves control.
	void setValves(BioXASValves *newControl);
	/// Sets the pre-mirror mask state control.
	void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMaskState(BioXASSSRLMonochromatorMaskState *newControl);

protected slots:
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

protected:
	/// The shutters.
	BioXASShutters *shutters_;
	/// The valves.
	BioXASValves *valves_;
	/// The pre-mirror mask.
	BioXASM1MirrorMaskState *mirrorMaskState_;
	/// The pre-mono mask.
	BioXASSSRLMonochromatorMaskState *monoMaskState_;
};

#endif // BIOXASBEAMSTATUS_H
