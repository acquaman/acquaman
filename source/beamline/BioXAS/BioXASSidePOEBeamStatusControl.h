#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROL_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASBeamStatusControl.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h"

class BioXASSidePOEBeamStatusControl : public BioXASBeamStatusControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControl();

	/// Returns true if this control is always measurable, provided it is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided this control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const;

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const { return !isOn(); }

	/// Returns the front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatus() const { return frontEndBeamStatus_; }
	/// Returns the pre-mirror mask control.
	AMControl* mirrorMask() const { return mirrorMask_; }
	/// Returns the pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl* monoMask() const { return monoMask_; }

signals:
	/// Notifier that the front-end beam status control has changed.
	void frontEndBeamStatusChanged(AMControl *newControl);
	/// Notifier that the pre-mirror mask control has changed.
	void mirrorMaskChanged(AMControl *newControl);
	/// Notifier that the pre-mono mask control has changed.
	void monoMaskChanged(AMControl *newControl);

public slots:
	/// Sets the front-end beam status control.
	void setFrontEndBeamStatus(BioXASFrontEndBeamStatusControl *newControl);
	/// Sets the pre-mirror mask control.
	void setMirrorMask(AMControl *newControl);
	/// Sets the pre-mono mask control.
	void setMonoMask(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// The front-end beam status control
	BioXASFrontEndBeamStatusControl *frontEndBeamStatus_;
	/// The pre-mirror (M1) mask control.
	AMControl *mirrorMask_;
	/// The pre-mono mask control.
	BioXASSSRLMonochromatorMaskControl *monoMask_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROL_H
