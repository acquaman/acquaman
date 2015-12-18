#ifndef BIOXASSIDEPOEBEAMSTATUS_H
#define BIOXASSIDEPOEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"

class BioXASM1MirrorMaskState;

class BioXASSidePOEBeamStatus : public BioXASFrontEndBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatus();

	/// Returns true if this control is connected, false otherwise. Reimplemented to consider additionally the POE-specific controls.
	virtual bool isConnected() const;

	/// Returns the pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState* mirrorMaskState() const { return mirrorMaskState_; }
	/// Returns the pre-mono mask state control.
	AMControl* monoMaskState() const { return monoMaskState_; }

signals:
	/// Notifier that the pre-mirror mask state control has changed.
	void mirrorMaskStateChanged(BioXASM1MirrorMaskState *newControl);
	/// Notifier that the pre-mono mask state control has changed.
	void monoMaskStateChanged(AMControl *newControl);

public slots:
	/// Sets the pre-mirror mask state control.
	virtual void setMirrorMaskState(BioXASM1MirrorMaskState *newControl);
	/// Sets the pre-mono mask state control.
	virtual void setMonoMaskState(AMControl *newControl);

protected:
	/// The pre-mirror (M1) mask state control.
	BioXASM1MirrorMaskState *mirrorMaskState_;
	/// The pre-mono mask state control.
	AMControl *monoMaskState_;
};

#endif // BIOXASSIDEPOEBEAMSTATUS_H
