#ifndef AMPIC887EPICSCOORDINATOR_H
#define AMPIC887EPICSCOORDINATOR_H

#include <QObject>
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "AMPIC887Controller.h"

/*!
  * A class which coordinates communication between an AMPIC887Controller instance
  * and the epics layer of PVs.
  * NOTE: This is currently specialized for SGM's controller. Need to figure out
  * a means of generalizing this for more controllers per driver, that aren't tied
  * to SGM's PVs
  */
class AMPIC887EpicsCoordinator : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an epics coordinator which will coordinate between
	  * a set of standard PVs and the provided controller. The PVs will be initialized
	  * based on the current state of the controller.
	  * \param controller ~ The controller who will be passed the values pushed
	  * on the 'set' style PVs, and which will serve the data for the 'get' style
	  * PVs.
	  */
	explicit AMPIC887EpicsCoordinator(AMPIC887Controller* controller, QObject *parent = 0);

signals:

public slots:
protected slots:
	/*!
	  * Slot which handles signals indicating that a position update has been
	  * received from the controller. Pushes the passed newPosition values onto
	  * the respective axis feedback PVs.
	  * \param newPositions ~ A map of axes to their updated positions.
	  */
	void onPositionUpdate(const AMPIC887AxisMap<double>& newPositions);

	/*!
	  * Slot which handles signals indicating that the system velocity update has
	  * been received from the controller. Pushes the received value onto the
	  * velocity feedback PV.
	  * \param systemVelocity ~ The controller's new system velocity.
	  */
	void onSystemVelocityChanged(double systemVelocity);

	/*!
	  * Slot which handles signals indicating the a movement had begun on the controller.
	  * Sets the status PVs of the relevant axes to MoveActive.
	  * \param movementStatuses ~ Flags which indicate the current movement statuses
	  * of all the axes.
	  */
	void onMotionStartedChanged(AMGCS2::AxisMovementStatuses movementStatuses);

	/*!
	  * Slot which handles signals indicated that a motion has been completed on
	  * the controller. Sets the status PVs of all axes to MoveDone
	  */
	void onMotionCompleted();

	/*!
	  * Slot which handles signals indicating that a motion on the controller has
	  * failed. Updates the status PVs of the moving axes to Error.
	  * \param movementStatuses ~ Flags which indicate the axis movement statuses
	  * prior to the failure of the motion.
	  */
	void onMotionFailed(AMGCS2::AxisMovementStatuses movementStatuses);

	/*!
	  * Slot which handles a value being pushed to the X Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which has been pushed onto the setpoint
	  * PV.
	  */
	void onXAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the Y Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which has been pushed onto the setpoint
	  * PV.
	  */
	void onYAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the Z Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which has been pushed onto the setpoint
	  * PV.
	  */
	void onZAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the U Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which has been pushed onto the setpoint
	  * PV.
	  */
	void onUAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the V Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which has been pushed onto the setpoint
	  * PV.
	  */
	void onVAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the W Axis setpoint PV. Passes
	  * the movement request to the controller in order that it can attempt to
	  * begin the move.
	  * \param setpoint ~ The position value which was pushed onto the setpoint
	  * PV.
	  */
	void onWAxisSetpointChanged(double setpoint);

	/*!
	  * Slot which handles a value being pushed to the system velocity setpoint
	  * PV. Passes the request to alter the system velocity on to the controller.
	  * \param value ~ The velocity value which was pushed onto the setpoint PV.
	  */
	void onSystemVelocitySetpointChanged(double value);

	/*!
	  * Slot which handles a value being pushed to the stopAll PV. Passes the stop
	  * instruction along to the controller.
	  * \param stopAllValue ~ The value which was pushed on the stop PV (IGNORED)
	  */
	void onStopAll(double stopAllValue);

	/*!
	  * Slot which handles a connection being established with all the PV controls.
	  * Pushes the current values stored in the controller onto the PVs, then sets
	  * up the signals to coordinate between the two.
	  */
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
