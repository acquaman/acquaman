#ifndef SGMENERGYPVCONTROL_H
#define SGMENERGYPVCONTROL_H

#include "beamline/AMPVControl.h"

#define SGMENERGYPVCONTROL_CLOSEDLOOP_TOLERANCE 0.25
#define SGMENERGYPVCONTROL_COORDINATED_TOLERANCE 10
class SGMEnergyPVControl : public AMPVwStatusControl
{
    Q_OBJECT
public:
    explicit SGMEnergyPVControl(QObject *parent = 0);

	/// Indicates that this control \em should (assuming it's connected) be calibrated.
	virtual bool shouldPerformCoordinatedMovement() const;

	/// Indicates that this control \em should (assuming it's connected) be able to perform continuous movements.
	virtual bool canPerformCoordinatedMovement() const;

	/// A list of actions which sets the parameters for a coordinated movement.
	virtual AMAction3* createSetParametersActions(double startPoint, double endPoint, double deltaTime);
	/// A list of actions which defines the steps required to initialize a coordinated movement.
	virtual AMAction3* createInitializeCoordinatedMovementActions();
	/// A list of actions which defines the steps required to trigger an initialized coordinated movement.
	virtual AMAction3* createStartCoordinatedMovementActions();
	/// A list of wait actions which, when complete, signify that a coordinated movement has been completed.
	virtual AMAction3* createWaitForCompletionActions();
signals:

public slots:
protected:
	double savedStartpoint_;
	double savedEndpoint_;
	double savedDeltaTime_;

	AMControl* coordinatedTarget_;
	AMControl* coordinatedDeltaTime_;
	AMControl* coordinatedStart_;

};

#endif // SGMENERGYPVCONTROL_H
