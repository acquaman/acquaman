#ifndef BIOXASSIDEPOEBEAMSTATUS_H
#define BIOXASSIDEPOEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h"

class BioXASM1MirrorMaskState;

class BioXASSidePOEBeamStatus : public BioXASFrontEndBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatus();

	/// Returns the pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState* mirrorMaskState() const { return mirrorMaskState_; }
	/// Returns the pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl* monoMask() const { return monoMask_; }

signals:
	/// Notifier that the pre-mirror mask state control has changed.
	void mirrorMaskStateChanged(BioXASM1MirrorMaskState *newControl);
	/// Notifier that the pre-mono mask control has changed.
	void monoMaskChanged(AMControl *newControl);

public slots:
	/// Sets the pre-mirror mask state control.
	virtual void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask control.
	virtual void setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl);

protected:
	/// The pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState *mirrorMaskState_;
	/// The pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl *monoMask_;
};

#endif // BIOXASSIDEPOEBEAMSTATUS_H
