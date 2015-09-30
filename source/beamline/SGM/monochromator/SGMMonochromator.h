#ifndef SGMMONOCHROMATOR_H
#define SGMMONOCHROMATOR_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMControl.h"
#include "beamline/AMControlSet.h"

/*!
 * A class which represents a standard control interface for working with the monochromator
 * energy. As well as the standard move(setpoint), the monochromator can also perform
 * a coordinated move(setpoint, time) motion.
 */
class SGMMonochromator : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    /*!
     * Creates an instance of an SGMMonochromator
     */
    SGMMonochromator(QObject *parent = 0);

    /*!
      * Whether this pseudo motor is expected to be able to return feedback.
      */
    bool shouldMeasure() const;

    /*!
      * Whether this pseudo motor is expected to be able to move.
      */
    bool shouldMove() const;

    /*!
      * Whether this pseudo motor is expected to be able to stop.
      */
    bool shouldStop() const;

    /*!
      * Whether this pseudo motor is able to return feedback at this moment.
      */
    bool canMeasure() const;

    /*!
      * Whether this pseudo motor is able to move at this moment.
      */
    bool canMove() const;

    /*!
      * Whether this pseudo motor is able to stop at this moment.
      */
    bool canStop() const;

    /*!
     * Whether the undulator is currently moving to track changes to the energy.
     */
    bool isUndulatorTracking() const;

    /*!
     * Changes the tracking state of the undulator. When off the undulator will
     * not move to its optimized position to track changes to the energy.
     * \param isTracking ~ Whether the undulator should track energy.
     */
    void setUndulatorTracking(bool isTracking);

    /*!
     * Whether the exit slit is currently moving to track changes to the energy.
     */
    bool isExitSlitTracking() const;

    /*!
     * Changes the tracking state of the exit slit. When off the exit slit will
     * not move to its optimized position to track changes to the energy.
     * \param isTracking ~ Whether the exit slit should track energy.
     */
    void setExitSlitTracking(bool isTracking);

    /*!
     * Moves the monochromator to produce the provided energy over the given trajectory
     * time period. Also coordinates movement of the tracked components to optimize
     * flux during the motion as best as is possible.
     * \param setpoint ~ The energy to which the monochromator should move.
     * \param trajectoryTime ~ The time which the motion should take
     * \return A failure explation which describes the type of failuire should the
     * motion not take place.
     */
    virtual FailureExplanation move(double setpoint, double trajectoryTime);
signals:
    /*!
     * Signal indicating that the undulator tracking has been altered.
     * \param isTracking ~ The new tracking state of the undulator.
     */
    void undulatorTrackingChanged(bool isTracking);

    /*!
     * Signal indicating that the exit slit tracking has been altered.
     * \param isTracking ~ The new tracking state of the exit slit.
     */
    void exitSlitTrackingChanged(bool isTracking);

protected slots:
    /*!
     * Handles signals from the undulator tracking PV indicating that its value
     * has been altered.
     * \param value ~ The new value of the undulator tracking PV.
     */
    void onUndulatorTrackingPvValueChanged(double value);

    /*!
     * Handles signals from the exit slit tracking PV indicating that its value
     * has been altered.
     * \param value ~ The new value of the exit slit tracking PV.
     */
    void onExitSlitTrackingPvValeuChanged(double value);

    /*!
     * Handles signals from the required controls set, indicating that its connection
     * state has altered.
     * \param isConnected ~ Whether the control set is connected.
     */
    void onRequiredControlsConnectionChanged(bool isConnected);

    /*!
     * Updates the connected state of the mono control.
     */
    void updateConnected();

    /*!
     * Updates the value of the mono control.
     */
    void updateValue();

    /*!
     * Updates the moving state of the mono control.
     */
    void updateMoving();
protected:

    /*!
     * Creates a move action which, when run, will attempt to move the mono energy
     * to the provided setpoint value.
     * \param setpoint ~ The energy value to which the mono will attempt to move.
     */
    AMAction3 *createMoveAction(double setpoint);

    /*!
     * Creates a move action which, when run, will attempt to move the mono energy
     * to the provided setpoint, over the provided period of time.
     * \param setpoint ~ The energy value to which the mono will attempt to move.
     * \param trajectoryTime ~ The period of time over which the move will take
     * place.
     */
    virtual AMAction3* createMoveAction(double setpoint, double trajectoryTime);

    AMControl* energyPv_;

    AMControl* gratingAngleVelocityPv_;
    AMControl* exitSlitVelocityPv_;
    AMControl* undulatorVelocityPv_;

    AMControl* undulatorTrackingPv_;
    AMControl* exitSlitTrackingPv_;

    AMControlSet* requiredControls_;
};

#endif // SGMMONOCHROMATOR_H
