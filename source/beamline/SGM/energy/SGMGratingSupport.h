#ifndef SGMGRATINGSUPPORT_H
#define SGMGRATINGSUPPORT_H

#include <qglobal.h>
/*!
  * Namespace containing enumerators and functions relating to the grating translation
  * on the SGM Beamline
  */
namespace SGMGratingSupport {

/*!
  * Enumerates the different selections for the grating translation.
  */
enum GratingTranslation {
    UnknownGrating = -1,
    LowGrating = 0,
    MediumGrating = 1,
    HighGrating = 2
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
 * The encoder steps target to apply in order to reach the provided grating translation
 * \param translationSelection ~ The grating translation whose encoder steps target
 * is to be returned.
 */
inline static double enumToEncoderSteps(GratingTranslation translationSelection)
{
    switch(translationSelection) {
    case LowGrating:
        return 61300;
    case MediumGrating:

    case HighGrating:

    default:
        return 0;
    }
}

/*!
 * The grating translation enum value which the provided encoder steps refers to.
 * \param encoderSteps ~ The encoder steps value whose grating translation enum
 * is to be returned.
 */
inline static GratingTranslation encoderStepsToEnum(double encoderSteps)
{
    // Todo: Verify this value?
    double tolerance = 10;
    if(qAbs(encoderSteps - 61300) < tolerance) {

        return LowGrating;
    }

    if(qAbs(encoderSteps - 0000) < tolerance) {

        return MediumGrating;
    }

    if(qAbs(encoderSteps - 0000) < tolerance) {

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

}

#endif // SGMGRATINGSUPPORT_H
