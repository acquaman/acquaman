#ifndef BIOXASFRONTENDBEAMSTATUSCONTROL_H
#define BIOXASFRONTENDBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASBeamStatusControl.h"
#include "beamline/BioXAS/BioXASValvesControl.h"

class BioXASFrontEndBeamStatusControl : public BioXASBeamStatusControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndBeamStatusControl(QObject *parent = 0);
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

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const;
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const;

	/// Returns the front-end upstream photon shutter.
	AMControl* photonShutterUpstream() const { return photonShutterUpstream_; }
	/// Returns the front-end downstream photon shutter.
	AMControl* photonShutterDownstream() const { return photonShutterDownstream_; }
	/// Returns the front-end safety shutter.
	AMControl* safetyShutter() const { return safetyShutter_; }
	/// Returns the valves.
	AMControl* valves() const { return valves_; }

signals:
	/// Notifier that the front-end upstream photon shutter changed.
	void photonShutterUpstreamChanged(AMControl *newControl);
	/// Notifier that the front-end downstream photon shutter changed.
	void photonShutterDownstreamChanged(AMControl *newControl);
	/// Notifier that the front-end safety shutter changed.
	void safetyShutterChanged(AMControl *newControl);
	/// Notifier that the front-end and beamline valves control has changed.
	void valvesChanged(AMControl *valves);

public slots:
	/// Sets the front-end upstream photon shutter.
	void setPhotonShutterUpstream(CLSBiStateControl *newControl);
	/// Sets the front-end downstream photon shutter.
	void setPhotonShutterDownstream(CLSBiStateControl *newControl);
	/// Sets the front-end safety shutter.
	void setSafetyShutter(CLSBiStateControl *newControl);
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
	/// The front-end upstream photon shutter.
	CLSBiStateControl *photonShutterUpstream_;
	/// The front-end downstream photon shutter.
	CLSBiStateControl *photonShutterDownstream_;
	/// The front-end safety shutter.
	CLSBiStateControl *safetyShutter_;
	/// The front-end and beamline valves.
	BioXASValvesControl *valves_;
};

#endif // BIOXASFRONTENDBEAMSTATUSCONTROL_H
