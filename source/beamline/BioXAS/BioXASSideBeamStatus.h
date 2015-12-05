#ifndef BIOXASSIDEBEAMSTATUS_H
#define BIOXASSIDEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"

class CLSBiStateControl;
class BioXAS

class BioXASSideBeamStatus : public BioXASBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatus(BioXASFrontEndShuttersControl *shutters, BioXASValvesControl *valves, AMControl *mirrorMask, BioXASSSRLMonochromatorMaskControl *monoMask, CLSBiStateControl *endstationShutter, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatus();

	/// Returns true if the valves are open, false otherwise.
	virtual bool isOn() const { return false; }
	/// Returns true if the valves are closed, false otherwise.
	virtual bool isOff() const { return false; }

signals:
	/// Notifier that the front-end shutters control has changed.
	void shuttersChanged(AMControl *newControl);
	/// Notifier that the front-end and beamline valves control has changed.
	void valvesChanged(AMControl *newControl);
	/// Notifier that the pre-mirror mask control has changed.
	void mirrorMaskChanged(AMControl *newControl);
	/// Notifier that the pre-mono mask control has changed.
	void monoMaskChanged(AMControl *newControl);
	/// Notifier that the endstation shutter control has changed.
	void endstationShutterChanged(AMControl *newControl);

public slots:
	/// Sets the shutters.
	void setShutters(BioXASFrontEndShuttersControl *newControl);
	/// Sets the valves.
	void setValves(BioXASValves *newControl);
	/// Sets the pre-mirror mask control.
	void setMirrorMask(AMControl *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl);
	/// Sets the endstation shutter control.
	void setEndstationShutter(CLSBiStateControl *newControl);
};

#endif // BIOXASSIDEBEAMSTATUS_H
