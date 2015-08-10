#ifndef AMPIC887EPICSCOORDINATOR_H
#define AMPIC887EPICSCOORDINATOR_H

#include <QObject>
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "AMPIC887Controller.h"
class AMPIC887EpicsCoordinator : public QObject
{
    Q_OBJECT
public:
	explicit AMPIC887EpicsCoordinator(AMPIC887Controller* controller, QObject *parent = 0);

signals:

public slots:
protected slots:
	void onPositionUpdate(const AMPIC887AxisMap<double>& newPositions);
	void onSystemVelocityChanged(double systemVelocity);
	void onMotionStartedChanged(AMGCS2::AxisMovementStatuses movementStatuses);
	void onMotionCompleted();
	void onMotionFailed();
	void onXAxisSetpointChanged(double setpoint);
	void onYAxisSetpointChanged(double setpoint);
	void onZAxisSetpointChanged(double setpoint);
	void onUAxisSetpointChanged(double setpoint);
	void onVAxisSetpointChanged(double setpoint);
	void onWAxisSetpointChanged(double setpoint);
	void onSystemVelocitySetpointChanged(double value);
	void onStopAll(double stopAllValue);
	void onAllConnected(bool connectedState);
protected:

	AMControlSet* allControls_;
	AMControl* xAxisValue_;
	AMControl* xAxisSetpoint_;
	AMControl* xAxisStatus_;
	AMControl* xAxisDriveHigh_;
	AMControl* xAxisDriveLow_;

	AMControl* yAxisValue_;
	AMControl* yAxisSetpoint_;
	AMControl* yAxisStatus_;
	AMControl* yAxisDriveHigh_;
	AMControl* yAxisDriveLow_;

	AMControl* zAxisValue_;
	AMControl* zAxisSetpoint_;
	AMControl* zAxisStatus_;
	AMControl* zAxisDriveHigh_;
	AMControl* zAxisDriveLow_;

	AMControl* uAxisValue_;
	AMControl* uAxisSetpoint_;
	AMControl* uAxisStatus_;
	AMControl* uAxisDriveHigh_;
	AMControl* uAxisDriveLow_;

	AMControl* vAxisValue_;
	AMControl* vAxisSetpoint_;
	AMControl* vAxisStatus_;
	AMControl* vAxisDriveHigh_;
	AMControl* vAxisDriveLow_;

	AMControl* wAxisValue_;
	AMControl* wAxisSetpoint_;
	AMControl* wAxisStatus_;
	AMControl* wAxisDriveHigh_;
	AMControl* wAxisDriveLow_;

	AMControl* systemVelocityValue_;
	AMControl* systemVelocitySetpoint_;

	AMControl* stopAll_;
	bool connectedOnce_;
	AMPIC887Controller* controller_;

	bool xAxisInitialMovement_;
	bool yAxisInitialMovement_;
	bool zAxisInitialMovement_;
	bool uAxisInitialMovement_;
	bool vAxisInitialMovement_;
	bool wAxisInitialMovement_;
	bool systemVelocityInitialMovement_;
};

#endif // AMPIC887EPICSCOORDINATOR_H
