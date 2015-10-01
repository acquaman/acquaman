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

signals:

public slots:

protected slots:
    // Connecting mono info controller to PVs
    void onMonoInfoGratingAngleChanged(double gratingAngle);
    void onMonoInfoGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);
    void onMonoInfoUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);
    void onMonoInfoUndulatorTrackingChanged(bool isTracking);
    void onMonoInfoUndulatorPositionChanged(double undulatorPosition);
    void onMonoInfoUndulatorOffsetChanged(double undulatorOffset);
    void onMonoInfoExitSlitTrackingChanged(bool exitSlitTracking);
    void onMonoInfoExitSlitPositionChanged(double exitSlitPosition);

    // Connectiong mono info energy readout to our pseudo motor value()
    void onMonoInfoEnergyChanged(double energy);

    // Connecting PVs to mono info controller
    void onGratingAnglePVValueChanged(double value);
    void onGratingTranslationPVValueChanged(double value);
    void onUndulatorHarmonicPVValueChanged(double value);
    void onUndulatorTrackingPVValueChanged(double value);
    void onUndulatorPositionPVValueChanged(double value);
    void onUndulatorOffsetPVValueChanged(double value);
    void onExitSlitTrackingPVValueChanged(double value);
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

    /// Mono info controller performs all the calculations, and coordination.
    SGMMonochromatorInfo* monoInfoController_;

    /// PVs for the mono components.
    AMControl* gratingAnglePV_;
    AMControl* gratingTranslationPV_;
    AMControl* undulatorHarmonicPV_;
    AMControl* undulatorTrackingPV_;
    AMControl* undulatorPositionPV_;
    AMControl* undulatorOffsetPV_;
    AMControl* exitSlitTrackingPV_;
    AMControl* exitSlitPositionPV_;
};

#endif // SGMMONOCHROMATORCONTROL_H
