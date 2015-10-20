#ifndef BIOXASFRONTENDBEAMSTATUSCONTROL_H
#define BIOXASFRONTENDBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASFrontEndShuttersControl.h"
#include "beamline/BioXAS/BioXASValvesControl.h"

class BioXASFrontEndBeamStatusControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatusControl();

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

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const { return !isOn(); }

	/// Returns a string representation of the given value.
	QString valueToString(BioXAS::Beam::Status value) const;

	/// Returns the front-end shutters.
	BioXASFrontEndShuttersControl* shutters() const { return shutters_; }
	/// Returns the front-end and beamline valves.
	BioXASValvesControl* valves() const { return valves_; }

signals:
	/// Notifier that the front-end shutters control has changed.
	void shuttersChanged(AMControl *newControl);
	/// Notifier that the front-end shutters upstream photon shutter has changed.
	void shuttersUpstreamPhotonShutterChanged(AMControl *newControl);
	/// Notifier that the front-end shutters downstream photon shutter has changed.
	void shuttersDownstreamPhotonShutterChanged(AMControl *newControl);
	/// Notifier that the front-end shutters safety shutter has changed.
	void shuttersSafetyShutterChanged(AMControl *newControl);
	/// Notifier that the front-end and beamline valves control has changed.
	void valvesChanged(AMControl *newControl);

public slots:
	/// Sets the shutters.
	void setShutters(BioXASFrontEndShuttersControl *newControl);
	/// Sets the valves.
	void setValves(BioXASValvesControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that changes the beam state.
	AMAction3* createMoveAction(double newState);

	/// Returns a new action that changes the control to 'beam on.'
	AMAction3* createBeamOnAction();
	/// Returns a new action that changes the control to 'beam off.'
	AMAction3* createBeamOffAction();

protected:
	/// The front-end shutters.
	BioXASFrontEndShuttersControl *shutters_;
	/// The front-end and beamline valves.
	BioXASValvesControl *valves_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROL_H
