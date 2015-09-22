#ifndef SGMGRATINGSUPPORT_H
#define SGMGRATINGSUPPORT_H

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
  * Spacing value for each of the grating translations used in the calculation
  * of the energy produced.
  * \param translationSelection ~ The grating whose spacing value is to be
  * returned.
  */
inline static double spacing(GratingTranslation translationSelection)
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
  * Correction value used in the calculation of the energy produced by a given
  * grating translation.
  * \param translationSelection ~ The grating whose correction value is to be
  * returned.
  */
inline static double c1(GratingTranslation translationSelection)
{
    switch(translationSelection) {
    case LowGrating:
        return 2.454768e-5;
    case MediumGrating:
        return 2.462036e-5;
    case HighGrating:
        return 2.456910e-5;
    default:
        return 0;
    }
}

/*!
  * Correction value used in the calculation of the energy produced by a given
  * grating translation.
  * \param translationSelection ~ The grating whose correction value is to be
  * returned.
  */
inline static double c2(GratingTranslation translationSelection)
{
    switch(translationSelection) {
    case LowGrating:
        return -1.593923;
    case MediumGrating:
        return -1.590466;
    case HighGrating:
        return -1.594012;
    default:
        return 0;
    }
}

/*!
  * Correction value used in the calculation of the energy produced by a given
  * grating translation.
  * \param translationSelection ~ The grating whose correction value is to be
  * returned.
  */
inline static double thetaM(GratingTranslation translationSelection)
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
