#ifndef BIOXASSIDEPOEBEAMSTATUS_H
#define BIOXASSIDEPOEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineBeamStatus.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h"

class BioXASSidePOEBeamStatus : public BioXASBeamlineBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatus(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatus();

	/// Returns the pre-mirror (M1) mask control.
	AMControl* mirrorMask() const { return mirrorMask_; }
	/// Returns the pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl* monoMask() const { return monoMask_; }

signals:
	/// Notifier that the pre-mirror mask control has changed.
	void mirrorMaskChanged(AMControl *newControl);
	/// Notifier that the pre-mono mask control has changed.
	void monoMaskChanged(AMControl *newControl);

public slots:
	/// Sets the pre-mirror mask control.
	void setMirrorMask(AMControl *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl);

protected slots:
	/// Updates the connected state. Reimplemented to consider particular children.
	virtual void updateConnected();

protected:
	/// The pre-mirror (M1) mask control.
	AMControl *mirrorMask_;
	/// The pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl *monoMask_;
};

#endif // BIOXASSIDEPOEBEAMSTATUS_H
