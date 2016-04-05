#ifndef SGMEXITSLITSUPPORT_H
#define SGMEXITSLITSUPPORT_H

#include "SGMGratingSupport.h"
#include <math.h>

#define MAX_EXIT_SLIT_POSITION 700.0
#define MIN_EXIT_SLIT_POSITION 1.0
/*!
  * Namespace containing functions relating to the SGM Exit Slit
  */
namespace SGMExitSlitSupport
{

/*!
  * Helper method used to calculate the exit slit position required to optimize
  * flux for a given energy and grating translation.
  * \param gratingTranslationSelection ~ The grating translation used in
  * calculating the optimized exit slit position.
  * \param energy ~ The energy to optimize the exit slit for.
  */
inline static double optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy)
{
	double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
	double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);
	double gratingEncoderOffset = 4600; // Distance between the grating and the zero position of the encoder
	double wavelength = (1239.842 / energy) * 1.0e-9;
	double angleOfIncidence = asin((wavelength / (2*gratingSpacing)) / cos(includedAngle/2)) + (includedAngle/2);
	double angleOfDefraction = -includedAngle + angleOfIncidence;

	double idealExitSlitPosition = -gratingEncoderOffset + (pow(cos(angleOfDefraction),2)) / (((cos(angleOfDefraction + includedAngle) + cos(angleOfDefraction)) / 70480) - (pow(cos(angleOfDefraction + includedAngle),2))/1500);

	return qBound(MIN_EXIT_SLIT_POSITION, idealExitSlitPosition, MAX_EXIT_SLIT_POSITION);
}

/*!
  * Helper function which returns the position at which the exit slit should
  * sit while a motion on energy from startEnergy to endEnergy is being performed
  * on the provided gratingTranslation
  */
inline static double exitSlitPositionForScan(double startEnergy, double endEnergy, SGMGratingSupport::GratingTranslation gratingTranslationSelection)
{
	// Currently just the mean. Speak with Tom Re: optimization
	double startPosition = optimizedExitSlitPosition(gratingTranslationSelection, startEnergy);
	double endPosition = optimizedExitSlitPosition(gratingTranslationSelection, endEnergy);

	return (startPosition + endPosition) / 2;
}

}

#endif // SGMEXITSLITSUPPORT_H
