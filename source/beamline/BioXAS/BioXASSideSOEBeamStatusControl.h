#ifndef BIOXASSIDESOEBEAMSTATUSCONTROL_H
#define BIOXASSIDESOEBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASBeamStatusControl.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatusControl.h"

class BioXASSideSOEBeamStatusControl : public BioXASBeamStatusControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatusControl();

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

	/// Returns the Side POE beam status control.
	BioXASSidePOEBeamStatusControl* poeBeamStatus() const { return poeBeamStatus_; }
	/// Returns the endstation shutter control.
	CLSBiStateControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the Side POE beam status control has changed.
	void poeBeamStatusChanged(BioXASSidePOEBeamStatusControl *newControl);
	/// Notifier that the endstation shutter control has changed.
	void endstationShutterChanged(AMControl *newControl);

public slots:
	/// Sets the Side POE beam status control.
	void setPOEBeamStatus(BioXASSidePOEBeamStatusControl *newControl);
	/// Sets the endstation shutter control.
	void setEndstationShutter(CLSBiStateControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Creates and returns an action that turns beam on.
	virtual AMAction3* createBeamOnAction();
	/// Creates and returns an action that turns beam off.
	virtual AMAction3* createBeamOffAction();

protected:
	/// The POE beam status.
	BioXASSidePOEBeamStatusControl *poeBeamStatus_;
	/// The endstation shutter.
	CLSBiStateControl *endstationShutter_;
};

#endif // BIOXASSIDESOEBEAMSTATUSCONTROL_H
