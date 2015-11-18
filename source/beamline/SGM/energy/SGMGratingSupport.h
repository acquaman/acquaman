#ifndef SGMGRATINGSUPPORT_H
#define SGMGRATINGSUPPORT_H

#include <qglobal.h>
#include <math.h>
/*!
  * Namespace containing enumerators and functions relating to the grating translation
  * on the SGM Beamline
  */
namespace SGMGratingSupport {

/*!
  * Enumerates the different selections for the grating translation.
  */
enum GratingTranslation {
	LowGrating = 0,
	MediumGrating = 1,
	HighGrating = 2,
	UnknownGrating = 3
};

/*!
  * Enumerates the different means of automatically selecting a Grating Translation.
  */
enum GratingTranslationOptimizationMode {

	ManualMode = 0,         // Stick with the current grating.
	OptimizeFlux = 1,		// Select grating which produces maximum flux for a given energy.
	OptimizeResolution = 2	// Select highest grating which will produce reasonable flux at a given energy.
};

/*!
  * The space between two consecutive rulings on a grating. Used in the calculation
  * of the energy produced.
  * \param translationSelection ~ The grating whose spacing value is to be
  * returned.
  */
inline static double gratingSpacing(GratingTranslation translationSelection)
{
	switch(translationSelection) {
	case LowGrating:
		return 1.689233e-6;
	case MediumGrating:
		return 9.163583e-7;
	case HighGrating:
		return 5.903703e-7;
	default:
		return 0;
	}
}

/*!
  * Correction value used to fit the theoretical energy values at a given grating
  * setting to the observed actual values.
  * \param translationSelection ~ The grating whose correction value is to be
  * returned.
  */
inline static double curveFitCorrection(GratingTranslation translationSelection)
{
	switch(translationSelection) {
	case LowGrating:
		return -3.912711175e-5;
	case MediumGrating:
		return -3.915784549e-5;
	case HighGrating:
		return -3.916344023e-5;
	default:
		return 0;
	}
}

/*!
  * The angle between the incident beam and the defracted beam. This value is
  * used in the calculation of the energy produced by the grating, as well as in
  * the calculation to determine the optimized exit slit position for a given
  * energy.
  * \param translationSelection ~ The grating whose included angle is to be
  * returned.
  */
inline static double includedAngle(GratingTranslation translationSelection)
{
	switch(translationSelection) {
	case LowGrating:
		return 3.055748;
	case MediumGrating:
		return 3.054777;
	case HighGrating:
		return 3.054579;
	default:
		return 0;
	}
}

/*!
  * Correction value based on the radius of the grating for each of the grating
  * translations, used in the calculation of the energy produced.
  * \param translationSelection ~ The translation whose radius curvature correction
  * is to be returned.
  */
inline static double radiusCurvatureOffset(GratingTranslation translationSelection)
{
	switch(translationSelection) {
	case LowGrating:
		return 5.0946788e2;
	case MediumGrating:
		return 5.112918e2;
	case HighGrating:
		return 5.104646e2;
	default:
		return 0;
	}
}

/*!
 * The encoder count target to apply in order to reach the provided grating translation
 * \param translationSelection ~ The grating translation whose encoder count target
 * is to be returned.
 */
inline static double enumToEncoderCount(GratingTranslation translationSelection)
{
	switch(translationSelection) {
	case LowGrating:
		return 61300;
	case MediumGrating:
		return 35100;
	case HighGrating:
		return 8807;
	default:
		return 0;
	}
}

/*!
 * The grating translation enum value which the provided encoder steps refers to.
 * \param encoderCount ~ The encoder count value whose grating translation enum
 * is to be returned.
 */
inline static GratingTranslation encoderCountToEnum(double encoderSteps)
{
	// Todo: Verify this value?
	double tolerance = 10;
	if(qAbs(encoderSteps - 61300) < tolerance) {

		return LowGrating;
	}

	if(qAbs(encoderSteps - 35100) < tolerance) {

		return MediumGrating;
	}

	if(qAbs(encoderSteps - 8807) < tolerance) {

		return HighGrating;
	}

	return UnknownGrating;
}

/*!
 * Checks whether the given energy is valid for the provided grating translation.
 * \param translationSelection ~ The grating translation to check against.
 * \param energy ~ The energy value to check the validity of for the given grating translation.
 */
inline static bool validEnergy(GratingTranslation translationSelection, double energy)
{
	switch(translationSelection) {
	case LowGrating:
		return (energy > 9);
	case MediumGrating:
		return (energy > 16);
	case HighGrating:
		return (energy > 25);
	default:
		return false;
	}
}

inline static SGMGratingSupport::GratingTranslation optimizedGratingTranslation(double requestedEnergy,
										GratingTranslationOptimizationMode optimizationMode)
{
	// The grating which produces the largest flux at a given photon energy.
	if(optimizationMode == OptimizeFlux) {

		if(requestedEnergy < 700) {

			return LowGrating;
		} else if(requestedEnergy < 1300) {

			return MediumGrating;
		} else {

			return HighGrating;
		}
	} else {

		if (requestedEnergy > 650) {

			return HighGrating;
		} else if (requestedEnergy > 450) {

			return MediumGrating;
		} else {

			return LowGrating;
		}
	}
}

/*!
  * Helper method used to determine the energy produced by a given grating
  * translation and angle encoder target.
  * \param gratingTranslationSelection ~ The grating translation to use in
  * calculating the produced energy.
  * \param gratingAngleEncoderTarget ~ The encoder target of the grating angle
  * to use in calculating the energy produced.
  */
inline static double energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
				       double gratingAngleEncoderTarget)
{

	double spacing = gratingSpacing(gratingTranslationSelection);
	double curveFit = curveFitCorrection(gratingTranslationSelection);
	double radiusCurvature = radiusCurvatureOffset(gratingTranslationSelection);
	double angle = includedAngle(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * curveFit * gratingAngleEncoderTarget) / radiusCurvature * cos(angle / 2));
}

/*!
  * Helper method which calculates the grating angle encoder target required
  * to produce the provided energy when using the provided grating translation.
  * \param gratingTranslationSelection ~ The grating translation under which
  * the energy is required.
  * \param energy ~ The energy for which the grating angle encoder target is
  * required.
  */
inline static double gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
					    double energy)
{
	double spacing = gratingSpacing(gratingTranslationSelection);
	double curveFit = curveFitCorrection(gratingTranslationSelection);
	double radiusCurvature = radiusCurvatureOffset(gratingTranslationSelection);
	double angle = includedAngle(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * curveFit * energy) / radiusCurvature * cos(angle / 2));
}

}

#endif // SGMGRATINGSUPPORT_H
