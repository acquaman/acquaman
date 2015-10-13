#ifndef BIOXASSIDEBEAMSTATUSCONTROL_H
#define BIOXASSIDEBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASBeamStatusControl.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"

class BioXASSideBeamStatusControl : public BioXASBeamStatusControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideBeamStatusControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatusControl();

	/// Returns true if this control is always measurable, provided it is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided this control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const;

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const { return !isOn(); }

	/// Returns the front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatusControl() const { return frontEndBeamStatus_; }
	/// Returns the pre-mirror mask control.
	AMControl* preMirrorMaskControl() const { return preMirrorMask_; }
	/// Returns the pre-mono mask control.
	AMControl* preMonoMaskControl() const { return preMonoMask_; }

signals:
	/// Notifier that the front-end beam status control has changed.
	void frontEndBeamStatusControlChanged(BioXASFrontEndBeamStatusControl *newControl);
	/// Notifier that the pre-mirror mask control has changed.
	void preMirrorMaskControlChanged(AMControl *newControl);
	/// Notifier that the pre-mono mask control has changed.
	void preMonoMaskControlChanged(AMControl *newControl);

public slots:
	/// Sets the front-end beam status control.
	void setFrontEndBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl);
	/// Sets the pre-mirror mask control.
	void setPreMirrorMaskControl(AMControl *newControl);
	/// Sets the pre-mono mask control.
	void setPreMonoMaskControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// The front-end beam status control
	BioXASFrontEndBeamStatusControl *frontEndBeamStatus_;
	/// The pre-mirror (M1) mask.
	AMControl *preMirrorMask_;
	/// The pre-mono mask.
	AMControl *preMonoMask_;
};

#endif // BIOXASSIDEBEAMSTATUSCONTROL_H
