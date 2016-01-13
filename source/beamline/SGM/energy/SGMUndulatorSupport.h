#ifndef SGMUNDULATORSUPPORT_H
#define SGMUNDULATORSUPPORT_H

#define UNDULATOR_STEP_TO_POSITION_SLOPE 8.467394369e-5
#define UNDULATOR_MIN_POSITION 12.51
#define UNDULATOR_MAX_POSITION 400.0
#include "SGMGratingSupport.h"
/*!
  * Namespace containing enumerators and functions related to the undulator on the
  * SMG Beamline
  */
namespace SGMUndulatorSupport
{

/*!
  * Enumerates the different harmonics for the SGM undulator (NOTE: Second harmonic
  * causes destructive interference, and is not used).
  */
enum UndulatorHarmonic {
    UnknownUndulatorHarmonic = -1,
    FirstHarmonic = 1,
    ThirdHarmonic = 3
};

/*!
  * Converts from the value of the undulator's mm position to the value used by
  * its motor step PV.
  * The function which maps position to steps is linear - with an intercept which
  * may change (for instance if the PV on the IOC is reset). In this case the
  * intercept value may change, but the slope will always remain the same. As such
  * it is required to provide known values for a position and its corresponding
  * step, so that the intercept for the current form can be calculated.
  * \param undulatorPosition ~ The mm position of the undulator to be converted.
  * \param knownUndulatorPosition ~ An undulator position for which the corresponding
  * step value is known.
  * \param correspondingUndulatorStep ~ An undulator step value which is known to
  * produce the above knownUndulatorPosition
  */
inline static double undulatorStepFromPosition(double undulatorPosition,
                                               double knownUndulatorPosition,
                                               double correspondingUndulatorStep)
{
    double intercept = knownUndulatorPosition - UNDULATOR_STEP_TO_POSITION_SLOPE * correspondingUndulatorStep;
    return (undulatorPosition - intercept) / UNDULATOR_STEP_TO_POSITION_SLOPE;
}

/*!
  * Converts from the value used by the undulators motor step PV into a mm position.
  * The function which maps steps to position is linear - with an intercept which
  * may change (for instance if the PV on the IOC is reset). In this case the
  * intercept value may change, but the slope will always remain the same. As such
  * it is required to provide known values for a position and its corresponding
  * step, so that the intercept for the current form can be calculated.
  * \param undulatorStep ~ The step value to be converted to a mm position.
  * \param knownUndulatorPosition ~ An undulator position for which the corresponding
  * step value is known.
  * \param correspondingUndulatorStep ~ An undulator step value which is known to
  * produce the above knownUndulatorPosition
  */
inline static double undulatorPositionFromStep(double undulatorStep,
                                               double knownUndulatorPosition,
                                               double correspondingUndulatorStep)
{
    double intercept = knownUndulatorPosition - UNDULATOR_STEP_TO_POSITION_SLOPE * correspondingUndulatorStep;
    return UNDULATOR_STEP_TO_POSITION_SLOPE * undulatorStep + intercept;
}

/*!
 * Checks whether a given energy is valid for the provided undulator harmonic.
 * \param undulatorHarmonic ~ The undulator to check against.
 * \param energy ~ The energy value to check the validity of for the given harmonic.
 */
inline static bool validEnergy(UndulatorHarmonic undulatorHarmonic, double energy)
{
    switch(undulatorHarmonic) {
    case FirstHarmonic:
        return (energy > 47 && energy < 1738);
    case ThirdHarmonic:
        return (energy > 141 && energy < 5213);
    default:
        return false;
    }
}

/*!
  * Helper method used to calculate the undulator (step) postition required
  * to optimize flux for a given energy and harmonic taking into account the
  * provided undulator detune offset.
  * \param energy ~ The energy which the calculated undulator step position
  * is to be optimized for.
  * \param undulatorHarmonic ~ The undulator harmonic to use in calculating
  * the optimized undulator position.
  * \param undulatorOffset ~ A detune value offset for the produced undulator
  * position.
  */
inline static double optimizedUndulatorPosition(double energy,
                                                SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
                                                double undulatorOffset)
{
	double rawUndulatorPosition = (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;

	if(energy > 1737.41045746644 && undulatorHarmonic == SGMUndulatorSupport::FirstHarmonic) {
		 rawUndulatorPosition = UNDULATOR_MAX_POSITION + undulatorOffset;
	}
	return qBound(UNDULATOR_MIN_POSITION, rawUndulatorPosition, UNDULATOR_MAX_POSITION);
}

/*!
 * Helper method used to calcualte the undulator harmonic required to optimize
 * flux for a given energy and grating translation.
 * \param gratingTranslationSelection ~ The grating translation used in
 * calculating the optimized undulator harmonic.
 * \param energy ~ The energy to optimize the undulator harmonic for.
 * \return
 */
inline static UndulatorHarmonic optimizedUndulatorHarmonic(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
							   double energy)
{
	if(gratingTranslationSelection == SGMGratingSupport::HighGrating &&
		energy >= 1400) {

		return ThirdHarmonic;
	} else {

		return FirstHarmonic;
	}
}

}

#endif // SGMUNDULATORSUPPORT_H
