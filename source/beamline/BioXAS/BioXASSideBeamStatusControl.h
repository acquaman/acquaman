#ifndef BIOXASSIDEBEAMSTATUSCONTROL_H
#define BIOXASSIDEBEAMSTATUSCONTROL_H

#include "beamline/BioXAS/BioXASBeamStatusControl.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatusControl.h"
#include "beamline/BioXAS/BioXASSideSOEBeamStatusControl.h"

class BioXASSideBeamStatusControl : public BioXASBeamStatusControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideBeamStatusControl();

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

	/// Returns the front-end beam status control.
	BioXASFrontEndBeamStatusControl* frontEndBeamStatus() const { return frontEndStatus_; }
	/// Returns the POE beam status control.
	BioXASSidePOEBeamStatusControl* poeBeamStatus() const { return poeBeamStatus_; }
	/// Returns the SOE beam status control.
	BioXASSideSOEBeamStatusControl* soeBeamStatus() const { return soeBeamStatus_; }

signals:
	/// Notifier that the front-end beam status control has changed.
	void frontEndBeamStatusChanged(AMControl *newControl);
	/// Notifier that the POE beam status control has changed.
	void poeBeamStatusChanged(AMControl *newControl);
	/// Notifier that the SOE beam status control has changed.
	void soeBeamStatusChanged(AMControl *newControl);

public slots:
	/// Sets the front-end beam status control.
	void setFrontEndBeamStatus(BioXASFrontEndBeamStatusControl *newControl);
	/// Sets the POE beam status control.
	void setPOEBeamStatus(BioXASSidePOEBeamStatusControl *newControl);
	/// Sets the SOE beam status control.
	void setSOEBeamStatus(BioXASSideSOEBeamStatusControl *newControl);

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
	/// The front-end beam status.
	BioXASFrontEndBeamStatusControl *frontEndStatus_;
	/// The POE beam status.
	BioXASSidePOEBeamStatusControl *poeBeamStatus_;
	/// The SOE beam status.
	BioXASSideSOEBeamStatusControl *soeBeamStatus_;
};

#endif // BIOXASSIDEBEAMSTATUSCONTROL_H
