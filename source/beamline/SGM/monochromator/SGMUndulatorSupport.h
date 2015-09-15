#ifndef SGMUNDULATORSUPPORT_H
#define SGMUNDULATORSUPPORT_H

#define UNDULATOR_STEP_TO_POSITION_SLOPE 8.467394369e-5
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

}

#endif // SGMUNDULATORSUPPORT_H
