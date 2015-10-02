#ifndef SGMMONOCHROMATORCONTROL_H
#define SGMMONOCHROMATORCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "SGMGratingSupport.h"
#include "SGMUndulatorSupport.h"

class SGMMonochromatorInfo;
/*!
 * A class which represents a coordinated motor for setting the SGM beamline's
 * monochromator energy.
 * This class is used in the coordinator application in order to leverage the
 * logic within the SGMMonochromatorInfo and AMPseudoMotorControl classes, making
 * the coordination task simpler.
 */
class SGMMonochromatorControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    /*!
     * Creates an instance of an SGMMonochromatorControl, tracking changes in
     * the mono component PVs, and coordinating them into a single energy readout.
     */
    explicit SGMMonochromatorControl(QObject *parent = 0);

    /*!
     * Whether the undulator is currently tracking the energy of the monochromator
     * in order to produce optimal flux.
     */
    bool isUndulatorTracking() const;

    /*!
     * Sets the tracking state of the undulator.
     * \param isTracking ~ Whether the undulator is to track the energy as it moves.
     */
    void setUndulatorTracking(bool isTracking);

    /*!
     * Whether the exit slit is currently tracking the energy of the monochromator
     * in order to produce optimal flux.
     */
    bool isExitSlitTracking() const;

    /*!
     * Sets the tracking state of the exit slit.
     * \param isTracking ~ Whether the exit slit is to track the energy as it moves.
     */
    void setExitSlitTracking(bool isTracking);
signals:

    /*!
     * Signal indicating a change in state of the undulator tracking.
     * \param isTracking ~ The new tracking state of the undulator.
     */
    void undulatorTrackingChanged(bool isTracking);

    /*!
     * Signal indicating a change in state of the exit slit tracking.
     * \param isTracking ~ The new tracking state of the exit slit.
     */
    void exitSlitTrackingChanged(bool isTracking);

public slots:

protected slots:
    // Connecting mono info controller to PVs
    void onMonoInfoGratingAngleChanged(double gratingAngle);
    void onMonoInfoGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);
    void onMonoInfoUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);
    void onMonoInfoUndulatorPositionChanged(double undulatorPosition);
    void onMonoInfoUndulatorOffsetChanged(double undulatorOffset);
    void onMonoInfoExitSlitPositionChanged(double exitSlitPosition);

    // Connectiong mono info energy readout to our pseudo motor value()
    void onMonoInfoEnergyChanged(double energy);

    // Connecting PVs to mono info controller
    void onGratingAnglePVValueChanged(double value);
    void onGratingTranslationPVValueChanged(double value);
    void onUndulatorHarmonicPVValueChanged(double value);
    void onUndulatorPositionPVValueChanged(double value);
    void onUndulatorOffsetPVValueChanged(double value);
    void onExitSlitPositionPVValueChanged(double value);


protected slots:
    /*!
     * Slot which handles connection changes to our component PVs
     */
    void updateConnected();

    /*!
     * Slot which handles value changes to our component PVs (does nothing)
     */
    void updateValue();

    /*!
     * Slot which handles changes to the moving state of our component PVs
     */
    void updateMoving();

protected:
    /*!
     * Creates a list of actions which coordinate the components of the monochromator
     * such that it will produce the provided energy setpoint.
     * \param setpoint ~ The energy value which the move action will move the energy
     * towards.
     */
    AMAction3* createMoveAction(double setpoint);

    /*!
     * Helper method to perform the initialization of the mono info controller
     * values once all the PVs have been connected.
     */
    void initializeMonoInfoController();

    /// Mono info controller which performs all the calculations, and coordination.
    SGMMonochromatorInfo* monoInfoController_;

    /// PVs for the mono components.
    AMControl* gratingAnglePV_;
    AMControl* gratingTranslationPV_;
    AMControl* undulatorHarmonicPV_;
    AMControl* undulatorPositionPV_;
    AMControl* undulatorOffsetPV_;
    AMControl* exitSlitPositionPV_;
};

#endif // SGMMONOCHROMATORCONTROL_H
