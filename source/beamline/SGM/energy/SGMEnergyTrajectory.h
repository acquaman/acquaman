#ifndef SGMENERGYTRAJECTORY_H
#define SGMENERGYTRAJECTORY_H

#include "SGMGratingSupport.h"
#include "SGMUndulatorSupport.h"
#include "SGMEnergyPosition.h"
#include "util/AMTrapezoidVelocityProfile.h"

#define SGMENERGYTRAJECTORY_INVALID_TIME "Invalid time for motion: Must be greater than 0"

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
	 * \param gratingAngleAcceleration ~ The acceleration value for the grating
	 * angle (in steps)
	 * \param gratingAngleStepsPerEncoderCount ~ The conversion rate between encoder
	 * counts and steps.
	 * \param undulatorAcceleration ~ The acceleration of the undulator (in steps)
	 * \param undulatorCurrentGap ~ The current undualtor gap
	 * \param undulatorCurrentStep ~ The current undulator steps
	 * \param undulatorHarmonic ~ The current undulator harmonic
	 */
	SGMEnergyTrajectory(double startEnergy,
	                    double endEnergy,
	                    double time,
	                    SGMGratingSupport::GratingTranslation gratingTranslation,
	                    double gratingAngleAcceleration,
	                    double gratingAngleStepsPerEncoderCount,
	                    double undulatorAcceleration,
	                    double undulatorCurrentGap,
	                    double undulatorCurrentStep,
	                    SGMUndulatorSupport::UndulatorHarmonic currentHarmonic);

	/*!
	  * Frees resources associated with the trajectory.
	  */
	~SGMEnergyTrajectory();

	/*!
	  * Convenience function for obtaining the error state of the energy trajectory.
	  */
	bool hasErrors() const;

	/*!
	  * Convenience funciton for obtaining the warning state of the energy
	  * trajectory.
	  */
	bool hasWarnings() const;

	/*!
	  * The error validator for the energy trajectory.
	  */
	AMValidator* errorValidator() const;

	/*!
	  * The warning validator for the energy trajectory.
	  */
	AMValidator* warningValidator() const;

	/*!
	 * The encoder step value of the grating angle at the start of the trajectory.
	 */
	double startGratingAngleEncoderCount() const;

	/*!
	 * The encoder step value of the grating angle at the end of the trajectory.
	 */
	double endGratingAngleEncoderCount() const;

	/*!
	 * The velocity profile of the grating angle to achieve the trajectory in the
	 * specified time.
	 */
	AMTrapezoidVelocityProfile gratingAngleVelocityProfile() const;

	/*!
	 * The position of the undulator at the start of the trajectory.
	 */
	double startUndulatorPosition() const;

	/*!
	 * The position of the undulator at the end of the trajectory.
	 */
	double endUndulatorPosition() const;

	/*!
	 * The velocity profile of the undulator to achieve the trajectory in the
	 * specified time.
	 */
	AMTrapezoidVelocityProfile undulatorVelocityProfile() const;

	/*!
	 * The position of the exit slit at the start of the trajectory.
	 */
	double startExitSlitPosition() const;

	/*!
	 * The position of the exit slit at the end of the trajectory.
	 */
	double endExitSlitPosition() const;

	/*!
	 * The velocity profile of the exit slit to achieve the trajectory in the
	 * specified time.
	 */
	AMTrapezoidVelocityProfile exitSlitVelocityProfile() const;

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

	/*!
	  * The velocity (in eV/s) which the trajectory will move at.
	  */
	double energyVelocity() const;

	/*!
	 * A string representation of the trajectory.
	 */
	QString toString() const;

protected:

	/*!
	  * Function which ensures all the error and warning checks are met.
	  */
	void performValidation();

	double startEnergy_;
	double endEnergy_;
	double time_;

	SGMGratingSupport::GratingTranslation gratingTranslation_;

	SGMEnergyPosition* startEnergyPosition_;
	SGMEnergyPosition* endEnergyPosition_;

	AMValidator* errorValidator_;
	AMValidator* warningValidator_;

	AMTrapezoidVelocityProfile gratingAngleVelocityProfile_;
	AMTrapezoidVelocityProfile undulatorVelocityProfile_;
	AMTrapezoidVelocityProfile exitSlitVelocityProfile_;
};

#endif // SGMENERGYTRAJECTORY_H
