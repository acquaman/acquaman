#ifndef SGMENERGYCONTROL_H
#define SGMENERGYCONTROL_H

#include <QObject>
#include "beamline/AMPseudoMotorControl.h"
#include "SGMEnergyPosition.h"

/*!
 * A class for controlling the energy of the SGM beamline, with coordinated movement
 * of the undulator position, exit slit position, grating angle and grating translation.
 */
class SGMEnergyControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    /*!
     * Creates a new energy control which is initialized with the provided undulator
     * harmonic.
     * \param undulatorHarmonic ~ The undulator harmonic to initialize the control
     * with.
     */
    SGMEnergyControl(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
                     QObject* parent = 0);

    /*!
     * The undulator harmonic which the control is currently using.
     */
    SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic() const;

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
    SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode() const;

    /*!
     * Whether the exit slit position should update as the energy moves.
     */
    bool isExitSlitPositionTracking() const;

	/*!
	  * The grating angle used to produce the current energy.
	  */
	double gratingAngle() const;

	/*!
	  * The grating translation used to produce the current energy.
	  */
	SGMGratingSupport::GratingTranslation gratingTranslation() const;

	/*!
	  * The current undulator position.
	  */
	double undulatorPosition() const;

	/*!
	  * The current exit slit position.
	  */
	double exitSlitPosition() const;
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
    void gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

    /*!
     * Signal indicating that the exit slit tracking state which the control is
     * using has been altered.
     * \param isTracking ~ The new exit slit tracking state.
     */
    void exitSlitTrackingChanged(bool isTracking);

	/*!
	  * Signal indicating that the grating angle used to produce the energy has
	  * been altered.
	  * \param gratingAngle ~ The new grating angle.
	  */
	void gratingAngleChanged(double gratingAngle);

	/*!
	  * Signal indicating that the grating translation used to produce an energy
	  * has been altered.
	  * \param gratingTranslation ~ The new grating translation.
	  */
	void gratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	  * Signal indicating that the undulator position has been changed.
	  * \param undulatorPosition ~ The new undulator position.
	  */
	void undulatorPositionChanged(double undulatorPosition);

	/*!
	  * Signal indicating that the exit slit postion has been changed.
	  * \param exitSlitPosition ~ The new exit slit position.
	  */
	void exitSlitPositionChanged(double exitSlitPosition);

public slots:
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
	void setGratingTranslationOptimizationMode(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

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

protected:

    /*!
     * Helper function which converts the grating translation enum to the double
     * value used by the grating translation PV value.
     * \param gratingTranslationEnum ~ The grating translation enum value whose
     * double equivalent is to be returned.
     */
    double gratingTranslationEnumToDouble(SGMGratingSupport::GratingTranslation gratingTranslationEnum);

    /*!
     * Helper function which converts the grating translation PV value to the
     * equivalent enum value.
     * \param gratingTranslationValue ~ The PV value whose enum equivalent is to
     * be returned.
     */
    SGMGratingSupport::GratingTranslation gratingTranslationDoubleToEnum(double gratingTranslationValue);

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
    virtual AMAction3* createMoveAction(double setpoint);

    SGMEnergyPosition* energyPositionController_;
    SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic_;

    AMControl* gratingAnglePV_;
    AMControl* gratingTranslationPV_;
    AMControl* undulatorPositionPV_;
    AMControl* exitSlitPositionPV_;
};

#endif // SGMENERGYCONTROL_H
