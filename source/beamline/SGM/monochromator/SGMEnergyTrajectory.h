#ifndef SGMENERGYTRAJECTORY_H
#define SGMENERGYTRAJECTORY_H

#include "SGMGratingSupport.h"
#include "SGMUndulatorSupport.h"
#include "SGMMonochromatorInfo.h"

/*!
 * A class representing a trajectory of motion along the energy of the SGM monochromator
 * over a length of time.
 */
class SGMEnergyTrajectory
{
public:
    /*!
     * Creates an instance of an SGMEnergyTrajectory which is to coordinate movement
     * from the start energy, to the end energy, on the provided grating translation
     * for the specified time period.
     * \param startEnergy ~ The start energy of the trajectory.
     * \param endEnergy ~ The end energy of the trajectory.
     * \param time ~ The time the move between energies will take.
     * \param gratingTranslation ~ The grating translation on which the movement
     * will take place.
     */
    SGMEnergyTrajectory(double startEnergy, double endEnergy, double time, SGMGratingSupport::GratingTranslation gratingTranslation);

    /*!
      * Frees resources associated with the trajectory.
      */
    ~SGMEnergyTrajectory();

    /*!
     * The encoder step value of the grating angle at the start of the trajectory.
     */
    double startGratingAngleEncoderStep() const;

    /*!
     * The encoder step value of the grating angle at the end of the trajectory.
     */
    double endGratingAngleEncoderStep() const;

    /*!
     * The velocity of the grating angle to achieve the trajectory in the specified
     * time.
     */
    double gratingAngleVelocity() const;

    /*!
     * The position of the undulator at the start of the trajectory.
     */
    double startUndulatorPosition() const;

    /*!
     * The position of the undulator at the end of the trajectory.
     */
    double endUndulatorPosition() const;

    /*!
     * The velocity of the undulator to achieve the trajectory in the specified
     * time.
     */
    double undulatorVelocity() const;

    /*!
     * The position of the exit slit at the start of the trajectory.
     */
    double startExitSlitPosition() const;

    /*!
     * The position of the exit slit at the end of the trajectory.
     */
    double endExitSlitPosition() const;

    /*!
     * The velocity of the exit slit to achieve the trajectory in the specified
     * time.
     */
    double exitSlitVelocity() const;

    /*!
     * The grating translation of the monochromator throughout the trajectory.
     */
    SGMGratingSupport::GratingTranslation gratingTranslation() const;

    /*!
     * The harmonic of the undulator throughout the trajectory.
     */
    SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic() const;

    /*!
     * The time specified for the trajectory to take.
     */
    double time() const;

    /*!
     * The start target energy.
     */
    double startEnergy()  const;

    /*!
     * The end target energy.
     */
    double endEnergy() const;

protected:
    double startEnergy_;
    double endEnergy_;
    double time_;
    SGMGratingSupport::GratingTranslation gratingTranslation_;

    SGMMonochromatorInfo* startMonoInfo_;
    SGMMonochromatorInfo* endMonoInfo_;

};

#endif // SGMENERGYTRAJECTORY_H
