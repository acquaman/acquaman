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
	enum GratingTranslationSelection {
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
	inline static double spacing(GratingTranslationSelection translationSelection)
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
	inline static double c1(GratingTranslationSelection translationSelection)
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
	inline static double c2(GratingTranslationSelection translationSelection)
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
	inline static double thetaM(GratingTranslationSelection translationSelection)
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
	inline static double radiusCurvatureOffset(GratingTranslationSelection translationSelection)
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
}

#endif // SGMGRATINGSUPPORT_H
