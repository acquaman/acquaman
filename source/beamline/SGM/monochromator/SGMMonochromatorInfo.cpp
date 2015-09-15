#include "SGMMonochromatorInfo.h"

#include <cmath>
SGMMonochromatorInfo::SGMMonochromatorInfo(QObject *parent) :
    QObject(parent)
{

    gratingTranslation_ = SGMGratingSupport::UnknownGrating;
    gratingAngle_ = 0;
    undulatorHarmonic_ = SGMUndulatorSupport::UnknownUndulatorHarmonic;
    undulatorPosition_ = 0;
    undulatorOffset_ = 0;
    exitSlitPosition_ = 0;
    requestedEnergy_ = 0;


    performValidation();

	connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
	connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged()));

	connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
	connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged()));
}

bool SGMMonochromatorInfo::hasErrors() const
{
	return !errorValidator_.isValid();
}

bool SGMMonochromatorInfo::hasWarnings() const
{
	return !warningValidator_.isValid();
}

QStringList SGMMonochromatorInfo::errorMessages() const
{
	return errorValidator_.failureMessages();
}

QStringList SGMMonochromatorInfo::warningMessages() const
{
	return warningValidator_.failureMessages();
}

double SGMMonochromatorInfo::resultantEnergy() const
{
	return energyFromGrating(gratingTranslation_, gratingAngle_);
}

SGMGratingSupport::GratingTranslation SGMMonochromatorInfo::gratingTranslation() const
{
	return gratingTranslation_;
}

void SGMMonochromatorInfo::setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation)
{
	if(gratingTranslation_ != gratingTranslation) {
        gratingTranslation_ = gratingTranslation;
        performValidation();
		emit gratingTranslationChanged(gratingTranslation_);
		emit energyChanged(resultantEnergy());
	}
}

double SGMMonochromatorInfo::gratingAngle() const
{
	return gratingAngle_;
}

void SGMMonochromatorInfo::setGratingAngle(double gratingAngle)
{
	if(gratingAngle_ != gratingAngle) {
		gratingAngle_ = gratingAngle;
        performValidation();
		emit gratingAngleChanged(gratingAngle_);
		emit energyChanged(resultantEnergy());
	}
}

SGMUndulatorSupport::UndulatorHarmonic SGMMonochromatorInfo::undulatorHarmonic() const
{
	return undulatorHarmonic_;
}

void SGMMonochromatorInfo::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(undulatorHarmonic_ != undulatorHarmonic) {
        undulatorHarmonic_ = undulatorHarmonic;
        performValidation();
		emit undulatorHarmonicChanged(undulatorHarmonic_);
	}
}

double SGMMonochromatorInfo::undulatorPosition() const
{
	return undulatorPosition_;
}

void SGMMonochromatorInfo::setUndulatorPosition(double undulatorPosition)
{
	if(undulatorPosition_ != undulatorPosition) {
		undulatorPosition_ = undulatorPosition;
        performValidation();
		emit undulatorPositionChanged(undulatorPosition);
	}
}

double SGMMonochromatorInfo::undulatorOffset() const
{
	return undulatorOffset_;
}

void SGMMonochromatorInfo::setUndulatorOffset(double undulatorOffset)
{
	if(undulatorOffset_ != undulatorOffset) {
		undulatorOffset_ = undulatorOffset;
        performValidation();
		emit undulatorOffsetChanged(undulatorOffset_);
	}
}

double SGMMonochromatorInfo::exitSlitPosition() const
{
	return exitSlitPosition_;
}

void SGMMonochromatorInfo::setExitSlitPosition(double exitSlitPosition)
{
	if(exitSlitPosition_ != exitSlitPosition) {
		exitSlitPosition_ = exitSlitPosition;
        performValidation();
		emit exitSlitPositionChanged(exitSlitPosition_);
	}
}

void SGMMonochromatorInfo::requestEnergy(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation)
{
    requestedEnergy_ = requestedEnergy;
	setGratingTranslation(gratingTranslation);

    setGratingAngle(gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
	setUndulatorPosition(optimizedUndulatorPosition(requestedEnergy, undulatorHarmonic_, undulatorOffset_));
	setExitSlitPosition(optimizedExitSlitPosition(gratingTranslation_, requestedEnergy));
}

void SGMMonochromatorInfo::requestEnergy(double requestedEnergy, GratingTranslationOptimizationMode optimizationMode)
{
	requestEnergy(requestedEnergy, optimizedGrating(requestedEnergy, optimizationMode));
}

SGMGratingSupport::GratingTranslation SGMMonochromatorInfo::optimizedGrating(double requestedEnergy, SGMMonochromatorInfo::GratingTranslationOptimizationMode optimizationMode)
{
	// The grating which produces the largest flux at a given photon energy.
	if(optimizationMode == OptimizeFlux) {

		if(requestedEnergy < 700) {

			return SGMGratingSupport::LowGrating;
		} else if(requestedEnergy < 1300) {

			return SGMGratingSupport::MediumGrating;
		} else {

			return SGMGratingSupport::HighGrating;
		}
	} else if(optimizationMode == OptimizeResolution) {

		if (requestedEnergy > 650) {

			return SGMGratingSupport::HighGrating;
		} else if (requestedEnergy > 450) {

			return SGMGratingSupport::MediumGrating;
		} else {

			return SGMGratingSupport::LowGrating;
		}
	} else {

		return gratingTranslation_;
	}
}

double SGMMonochromatorInfo::energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double gratingAngleEncoderTarget) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double c1 = SGMGratingSupport::c1(gratingTranslationSelection);
	double c2 = SGMGratingSupport::c2(gratingTranslationSelection);
	double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * c1 * c2 * gratingAngleEncoderTarget) / radiusCurvatureOffset * cos(thetaM / 2));
}

double SGMMonochromatorInfo::gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double c1 = SGMGratingSupport::c1(gratingTranslationSelection);
	double c2 = SGMGratingSupport::c2(gratingTranslationSelection);
	double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * c1 * c2 * energy) / radiusCurvatureOffset * cos(thetaM / 2));
}

double SGMMonochromatorInfo::optimizedUndulatorPosition(double energy, SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic, double undulatorOffset) const
{
	return (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;
}

double SGMMonochromatorInfo::optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);
	double xOffset = 4546; // Need to research this value. Seems to have been altered over time.
	double lambda = (1239.842 / energy) * 1.0e-9;
	double thetaI = asin((lambda / (2*spacing)) / cos(thetaM/2)) + (thetaM/2);
	double thetaD = -thetaM + thetaI;

    return -xOffset + (pow(cos(thetaD),2)) / (((cos(thetaD + thetaM) + cos(thetaD)) / 70480) - (pow(cos(thetaD + thetaM),2))/1500);
}

void SGMMonochromatorInfo::performValidation()
{
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_UNDULATOR_HARMONIC, undulatorHarmonic_ == SGMUndulatorSupport::UnknownUndulatorHarmonic);
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_GRATING_TRANSLATION, gratingTranslation_ == SGMGratingSupport::UnknownGrating);
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_HARMONIC, !SGMUndulatorSupport::validEnergy(undulatorHarmonic_, requestedEnergy_));
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_GRATING, !SGMGratingSupport::validEnergy(gratingTranslation_, requestedEnergy_));
}



