#ifndef AMPIC887EPICSCOORDINATOR_H
#define AMPIC887EPICSCOORDINATOR_H

#include <QObject>
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "AMPIC887Controller.h"
#include "AMPIC887TrajectoryCoordinator.h"

/*!
  * A class which coordinates communication between an AMPIC887Controller instance
  * and the epics layer of PVs.
  * NOTE: This is currently specialized for SGM's controller. Need to figure out
  * a means of generalizing this for more controllers per driver, that aren't tied
  * to SGM's PVs
  */
class AMWaveformBinningSinglePVControl;
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
	  * Slot which handles signals indicating that the target position of the axes
	  * has been updated. Pushes the target position values onto the setpoint PVs
	  * (including the trajectory setpoint contained within the trajectory coordinator).
	  * \param targetPositions ~ A map containing the target position of all axes
	  * of the hexapod.
	  */
	void onTargetPositionChanged(const AMPIC887AxisMap<double>& targetPositions);

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

	/*!
	  * Slot which handles the trajectory coordinator signalling that a trajectory
	  * move has been requested.
	  */
	void onTrajectoryMove();

	/*!
	  * Slot which handles the record rate setpoint PV value being changed
	  * \param value ~ The new value to which the PV was set.
	  */
	void onRecordRateSetpointPVChanged(double value);

	/*!
	  * Slot which handles the controller signalling that its record rate has
	  * been altered.
	  * \param value ~ The new record rate of the controller.
	  */
	void onControllerRecordRateChanged(double value);

	/*!
	  * Slot which handles the data recorder active PV being changed.
	  * \param value ~ The value written to the data recorder active PV
	  */
	void onDataRecorderActivePVChanged(double value);

	/*!
	  * Slot which handles the data recorder active state being altered on the
	  * controller.
	  * \param isActive ~ The new active state of the controller's data recorder.
	  */
	void onControllerDataRecorderActiveChanged(bool isActive);

protected:

	/*!
	  * Helper function which parses the recorded position data from the hexapod
	  * controller, and pushes this data to the waveform PV.
	  */
	void parseRecordedPositionData();

	AMControlSet* allControls_;

	AMControl* xAxisFeedback_;
	AMControl* xAxisStatus_;
	AMWaveformBinningSinglePVControl* xAxisRecorderFeedback_;

	AMControl* yAxisFeedback_;
	AMControl* yAxisStatus_;
	AMWaveformBinningSinglePVControl* yAxisRecorderFeedback_;

	AMControl* zAxisFeedback_;
	AMControl* zAxisStatus_;
	AMWaveformBinningSinglePVControl* zAxisRecorderFeedback_;

	AMControl* uAxisFeedback_;
	AMControl* uAxisStatus_;
	AMWaveformBinningSinglePVControl* uAxisRecorderFeedback_;

	AMControl* vAxisFeedback_;
	AMControl* vAxisStatus_;
	AMWaveformBinningSinglePVControl* vAxisRecorderFeedback_;

	AMControl* wAxisFeedback_;
	AMControl* wAxisStatus_;
	AMWaveformBinningSinglePVControl* wAxisRecorderFeedback_;

	AMControl* recordRateSetpoint_;
	AMControl* recordRateFeedback_;
	AMControl* dataRecorderActive_;
	AMWaveformBinningSinglePVControl* timeRecorderFeedback_;
	AMControl* systemVelocityFeedback_;
	AMControl* systemVelocitySetpoint_;

	AMControl* stopAll_;
	bool connectedOnce_;
	AMPIC887Controller* controller_;

	AMPIC887TrajectoryCoordinator* trajectoryCoordinator_;
};

#endif // AMPIC887EPICSCOORDINATOR_H
