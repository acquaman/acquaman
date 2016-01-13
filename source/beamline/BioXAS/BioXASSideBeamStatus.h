#ifndef BIOXASSIDEBEAMSTATUS_H
#define BIOXASSIDEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"

class BioXASFrontEndShutters;
class BioXASMasterValves;
class BioXASM1MirrorMaskState;
class BioXASSSRLMonochromatorMaskState;
class CLSBiStateControl;

class BioXASSideBeamStatus : public BioXASBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatus();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* frontEndShutters() const { return frontEndShutters_; }
	/// Returns the valves.
	BioXASMasterValves* valves() const { return valves_; }
	/// Returns the pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState* mirrorMaskState() const { return mirrorMaskState_; }
	/// Returns the pre-mono mask state control.
	BioXASSSRLMonochromatorMaskState* monoMaskState() const { return monoMaskState_; }
	/// Returns the endstation shutter control.
	CLSBiStateControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the front-end shutters have changed.
	void frontEndShuttersChanged(BioXASFrontEndShutters *newShutters);
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASMasterValves *newValves);
	/// Notifier that the pre-mirror mask state control has changed.
	void mirrorMaskStateChanged(BioXASM1MirrorMaskState *newControl);
	/// Notifier that the pre-mono mask state control has changed.
	void monoMaskStateChanged(BioXASSSRLMonochromatorMaskState *newControl);
	/// Notifier that the endstation shutter control has changed.
	void endstationShutterChanged(CLSBiStateControl *newControl);

public slots:
	/// Sets the front-end shutters control.
	void setFrontEndShutters(BioXASFrontEndShutters *newControl);
	/// Sets the valves control.
	void setValves(BioXASMasterValves *newControl);
	/// Sets the pre-mirror mask state control.
	void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMaskState(BioXASSSRLMonochromatorMaskState *newControl);
	/// Sets the endstation shutter control.
	void setEndstationShutter(CLSBiStateControl *newControl);

protected:
	/// The front-end shutters.
	BioXASFrontEndShutters *frontEndShutters_;
	/// The valves.
	BioXASMasterValves *valves_;
	/// The pre-mirror mask.
	BioXASM1MirrorMaskState *mirrorMaskState_;
	/// The pre-mono mask.
	BioXASSSRLMonochromatorMaskState *monoMaskState_;
	/// The endstation shutter.
	CLSBiStateControl *endstationShutter_;
};

#endif // BIOXASSIDEBEAMSTATUS_H
