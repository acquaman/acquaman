#ifndef BIOXASMAINBEAMSTATUS_H
#define BIOXASMAINBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"

class BioXASMainShutters;
class BioXASMasterValves;
class BioXASM1MirrorMaskState;
class BioXASSSRLMonochromatorMaskState;
class CLSBiStateControl;

class BioXASMainBeamStatus : public BioXASBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainBeamStatus();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the shutters.
	BioXASMainShutters* shutters() const { return shutters_; }
	/// Returns the valves.
	BioXASMasterValves* valves() const { return valves_; }
	/// Returns the pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState* mirrorMaskState() const { return mirrorMaskState_; }
	/// Returns the pre-mono mask state control.
	BioXASSSRLMonochromatorMaskState* monoMaskState() const { return monoMaskState_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged(BioXASMainShutters *newShutters);
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASMasterValves *newValves);
	/// Notifier that the pre-mirror mask state control has changed.
	void mirrorMaskStateChanged(BioXASM1MirrorMaskState *newControl);
	/// Notifier that the pre-mono mask state control has changed.
	void monoMaskStateChanged(BioXASSSRLMonochromatorMaskState *newControl);

public slots:
	/// Sets the shutters control.
	void setShutters(BioXASMainShutters *newControl);
	/// Sets the valves control.
	void setValves(BioXASMasterValves *newControl);
	/// Sets the pre-mirror mask state control.
	void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMaskState(BioXASSSRLMonochromatorMaskState *newControl);

protected:
	/// The shutters.
	BioXASMainShutters *shutters_;
	/// The valves.
	BioXASMasterValves *valves_;
	/// The pre-mirror mask.
	BioXASM1MirrorMaskState *mirrorMaskState_;
	/// The pre-mono mask.
	BioXASSSRLMonochromatorMaskState *monoMaskState_;
};

#endif // BIOXASMAINBEAMSTATUS_H
