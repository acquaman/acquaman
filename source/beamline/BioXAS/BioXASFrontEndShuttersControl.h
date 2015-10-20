#ifndef BIOXASFRONTENDSHUTTERSCONTROL_H
#define BIOXASFRONTENDSHUTTERSCONTROL_H

#include "application/BioXAS/BioXAS.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASFrontEndShuttersControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum providing the values this control can have.
	enum Value { Open = 0, Closed = 1, None = 2, Between = 3 };

	/// Constructor.
	explicit BioXASFrontEndShuttersControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShuttersControl();

	/// Returns true if the value is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if this control's value is Open, false otherwise.
	bool isOpen() const;
	/// Returns true if this control's value is Closed, false otherwise.
	bool isClosed() const;

	/// Returns a string representation of the given value.
	QString valueToString(BioXASFrontEndShuttersControl::Value value) const;

	/// Returns the upstream photon shutter control.
	AMControl* photonShutterUpstream() const { return photonShutterUpstream_; }
	/// Returns the downstream photon shutter control.
	CLSBiStateControl* photonShutterDownstream() const { return photonShutterDownstream_; }
	/// Returns the safety shutter control.
	CLSBiStateControl* safetyShutter() const { return safetyShutter_; }

signals:
	/// Notifier that the upstream photon shutter control has changed.
	void photonShutterUpstreamChanged(AMControl *newControl);
	/// Notifier that the downstream photon shutter control has changed.
	void photonShutterDownstreamChanged(AMControl *newControl);
	/// Notifier that the safety shutter control has changed.
	void safetyShutterChanged(AMControl *newControl);

public slots:
	/// Sets the upstream photon shutter control.
	void setUpstreamPhotonShutter(AMControl *newControl);
	/// Sets the downstream photon shutter control.
	void setDownstreamPhotonShutter(CLSBiStateControl *newControl);
	/// Sets the safety shutter control.
	void setSafetyShutter(CLSBiStateControl *newControl);

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action to the given setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns an action that opens all shutters.
	AMAction3* createOpenShuttersAction();
	/// Creates and returns an action that closes the shutters.
	AMAction3* createCloseShuttersAction();

protected:
	/// The upstream photon shutter.
	AMControl *photonShutterUpstream_;
	/// The downstream photon shutter.
	CLSBiStateControl *photonShutterDownstream_;
	/// The safety shutter.
	CLSBiStateControl *safetyShutter_;
};

#endif // BIOXASFRONTENDSHUTTERSCONTROL_H
