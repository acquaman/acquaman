#ifndef SGMENERGYCOORDINATORCONTROL_H
#define SGMENERGYCOORDINATORCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "SGMUndulatorSupport.h"
#include "SGMEnergyPosition.h"

#define SGMENERGYCONTROL_INVALID_STATE			851142
#define SGMENERGYCONTROL_ZERO_ENERGY_VELOCITY		851143
#define SGMENERGYCONTROL_CLOSEDLOOP_TOLERANCE		0.25
#define SGMENERGYCONTROL_COORDINATED_TOLERANCE		10

class SGMEnergyTrajectory;
class SGMGratingAngleControl;
class SGMUndulatorControl;
/*!
  * A class which presents a standard AMControl interface for performing coordinated
  * motions of the SGM beamline energy components.
  */
class SGMEnergyCoordinatorControl : public AMPseudoMotorControl
{
	Q_OBJECT
public:
	SGMEnergyCoordinatorControl(SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic,
	                 QObject* parent = 0);

	/*!
	  * Whether the control is designed to be able to measure values.
	  */
	bool shouldMeasure() const;

	/*!
	  * Whether the control is designed to be able to move.
	  */
	bool shouldMove() const;

	/*!
	  * Whether the control is designed to be able to stop.
	  */
	bool shouldStop() const;

	/*!
	  * Whether the control can measure values in its current state.
	  */
	bool canMeasure() const;

	/*!
	  * Whether the control can move in its current state.
	  */
	bool canMove() const;

	/*!
	  * Whether the control can stop in its current state.
	  */
	bool canStop() const;

	/*!
	 * The undulator harmonic which the control is currently using.
	 */
	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic() const;

	/*!
	 * The undulator offset to use detuning the undulator position.
	 */
	double undulatorOffset() const;

	/*!
	 * Whether the control should automatically update the undulator position as
	 * the energy moves.
	 */
	bool isUndulatorTracking() const;

	/*!
	 * The method to use in determining the grating translation to use for an
	 * energy.
	 */
	SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode() const;

	/*!
	 * Whether the exit slit position should update as the energy moves.
	 */
	bool isExitSlitPositionTracking() const;

	// Child Controls

	/*!
	 * The control for the grating angle encoder steps.
	 */
	AMControl* gratingAngleControl() const;

	/*!
	 * The control for the grating translation encoder steps.
	 */
	AMControl* gratingTranslationControl() const;

	/*!
	 * The control for the undulator position.
	 */
	AMControl* undulatorPositionControl() const;

	/*!
	 * The control for the exit slit position.
	 */
	AMControl* exitSlitPositionControl() const;

signals:

	/*!
	 * Signal indicating that the undulator harmonic which the control is using
	 * has been altered.
	 * \param undulatorHarmonic ~ The new undulator harmonic.
	 */
	void undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	 * Signal indicating that the undulator offset which the control is using has
	 * been altered.
	 * \param undulatorOffset ~ The new undulator detune offset.
	 */
	void undulatorOffsetChanged(double undulatorOffset);

	/*!
	 * Signal indicating that the undulator tracking state which the control is
	 * using has been altered.
	 * \param isTracking ~ The new undulator tracking state.
	 */
	void undulatorTrackingChanged(bool isTracking);

	/*!
	 * Signal indicating that the grating translation optimization mode used by
	 * the control has been altered.
	 * \param gratingTranslationOptimizationMode ~ The new grating translation
	 * optimization mode.
	 */
	void gratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

	/*!
	 * Signal indicating that the exit slit tracking state which the control is
	 * using has been altered.
	 * \param isTracking ~ The new exit slit tracking state.
	 */
	void exitSlitTrackingChanged(bool isTracking);

public slots:

	/*!
	  * Moves the controls in a coordinated manner in order to arrive at the
	  * setpoint energy at the target velocity.
	  * \param targetSetpoint ~ The position which the energy will move to.
	  * \param time ~ The time for the motion to take
	  */
	virtual FailureExplanation move(double targetSetpoint, double time);

	/*!
	 * Sets the undulator harmonic to use in calculating the control positions.
	 * \param undulatorHarmonic ~ The undulator harmonic to use in calculating
	 * the control positions.
	 */
	void setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	 * Set the undulator detune offset.
	 * \param undulatorOffset ~ The new undulator detune offset.
	 */
	void setUndulatorOffset(double undulatorOffset);

	/*!
	 * Sets whether the undulator position should update as the energy moves.
	 * \param isTracking ~ The new undulator tracking state.
	 */
	void setUndulatorTracking(bool isTracking);

	/*!
	 * Sete the method to use in automatically determining the grating translation
	 * to use when setting an energy
	 * \param gratingTranslationOptimizationMode ~ The method used in determining
	 * the grating translation.
	 */
	void setGratingTranslationOptimizationMode(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

	/*!
	 * Sets whether the exit slit position should update as the energy moves.
	 * \param isTracking ~ The new exit slit tracking state.
	 */
	void setExitSlitPositionTracking(bool isTracking);
protected slots:

	/*!
	 * Slot which updates the connected state of the control.
	 */
	void updateConnected();

	/*!
	 * Slot which updates the moving state of the control.
	 */
	void updateMoving();

	/*!
	 * Slot which updates the value of the control.
	 */
	void updateValue();

	/*!
	 * Slot which updates the grating translation of the energy position controller
	 * as the grating translation PV changes.
	 * \param value ~ The value of the grating translation PV.
	 */
	void onGratingTranslationPVValueChanged(double value);

	/*!
	 * Slot which updates the value of this control as the calculated energy position
	 * controller's energy value changes
	 * \param value ~ The energy value calculated by the energy position controller.
	 */
	void onEnergyPositionControllerEnergyChanged(double value);

	/*!
	  * Slot which updates the undulator position whenever the undulator harmonic
	  * changes.
	  * \param undulatorHarmonic ~ The new undulator harmonic.
	  */
	void onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	  * Slot which handles the undulator position offset being changed
	  */
	void onEnergyPositionUndulatorOffsetChanged(double value);

	/*!
	  * Slot which handes updating the grating translation position each time the
	  * grating optimization mode changes.
	  * \param optimizationMode ~ The new grating optimization mode.
	  */
	void onEnergyPositionGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode optimizationMode);

protected:

	/*!
	 * Helper function which initializes the energy position controller once the
	 * PV's it derives its values from are connected.
	 */
	void initializeEnergyPositionController();

	/*!
	 * Creates a move action which will perform a coordinated motion of the
	 * energy beamline components.
	 * \param setpoint ~ The energy value to move the controls to.
	 */
	AMAction3* createMoveAction(double setpoint);

	/*!
	 * Creates a move action which will perform a coordinated motion of the
	 * energy beamline components such that they all arrive at their destination
	 * at the same time.
	 * \param energyTrajectory ~ The energy trajectory for the move action.
	 */
	virtual AMAction3* createMoveAction(SGMEnergyTrajectory* energyTrajectory);

	/*!
	  * Creates an action which returns the Energy Coordinator Control back to its defaults.
	  */
	virtual AMAction3* createDefaultsAction();

	SGMEnergyPosition* energyPositionController_;
	SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic_;

	// Child Controls for simply motions
	SGMGratingAngleControl* gratingAngleControl_;
	AMControl* gratingTranslationStepControl_;
	SGMUndulatorControl* undulatorControl_;
	AMControl* exitSlitPositionControl_;
};

#endif // SGMENERGYCOORDINATORCONTROL_H
