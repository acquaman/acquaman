#ifndef BIOXASSHUTTERSCONTROL_H
#define BIOXASSHUTTERSCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASShuttersControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum describing the different possible shutter states.
	enum State { Open = 0, Closed = 1, Between = 2, None = 3};
	/// Constructor.
	explicit BioXASShuttersControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersControl();

	/// Returns true if the region is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new region is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a crystal change in progress, provided it is connected.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a change to a new region right now.
	virtual bool canStop() const { return false; }

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the front-end upstream photon shutter.
	AMControl* photonShutterUpstreamFE() const { return photonShutterUpstreamFE_; }
	/// Returns the front-end downstream photon shutter.
	AMControl* photonShutterDownstreamFE() const { return photonShutterDownstreamFE_; }
	/// Returns the front-end safety shutter.
	AMControl* safetyShutterFE() const { return safetyShutterFE_; }
	/// Returns the endstation safety shutter.
	AMControl* safetyShutterES() const { return safetyShutterES_; }

signals:
	/// Notifier that the front-end upstream photon shutter changed.
	void photonShutterUpstreamFEChanged(AMControl *newControl);
	/// Notifier that the front-end downstream photon shutter changed.
	void photonShutterDownstreamFEChanged(AMControl *newControl);
	/// Notifier that the front-end safety shutter changed.
	void safetyShutterFEChanged(AMControl *newControl);
	/// Notifier that the endstation safety shutter changed.
	void safetyShutterESChanged(AMControl *newControl);

public slots:
	/// Sets the front-end upstream photon shutter.
	void setPhotonShutterUpstreamFE(CLSBiStateControl *newControl);
	/// Sets the front-end downstream photon shutter.
	void setPhotonShutterDownstreamFE(CLSBiStateControl *newControl);
	/// Sets the front-end safety shutter.
	void setSafetyShutterFE(CLSBiStateControl *newControl);
	/// Sets the endstation safety shutter.
	void setSafetyShutterES(CLSBiStateControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that changes the shutters state.
	AMAction3* createMoveAction(double newState);

protected:
	/// The front-end upstream photon shutter.
	CLSBiStateControl *photonShutterUpstreamFE_;
	/// The front-end downstream photon shutter.
	CLSBiStateControl *photonShutterDownstreamFE_;
	/// The front-end safety shutter.
	CLSBiStateControl *safetyShutterFE_;
	/// The endstation safety shutter.
	CLSBiStateControl *safetyShutterES_;
};

#endif // BIOXASSHUTTERCONTROL_H
