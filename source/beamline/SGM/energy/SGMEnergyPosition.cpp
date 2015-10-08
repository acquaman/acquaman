#include "SGMEnergyPosition.h"

#include <cmath>
SGMEnergyPosition::SGMEnergyPosition(SGMGratingSupport::GratingTranslation gratingTranslation,
                                           double gratingAngle,
                                           SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
                                           double undulatorPosition,
                                           double undulatorOffset,
                                           double exitSlitPosition,
                                           QObject *parent) :
    QObject(parent)
{
    gratingTranslation_ = gratingTranslation;
    gratingTranslationOptimizationMode_ = ManualMode;
    gratingAngle_ = gratingAngle;
    undulatorHarmonic_ = undulatorHarmonic;
    undulatorPosition_ = undulatorPosition;
    undulatorOffset_ = undulatorOffset;
    exitSlitPosition_ = exitSlitPosition;

    isUndulatorTracking_ = true;
    isExitSlitPositionTracking_ = true;
    autoDetectUndulatorHarmonic_ = true;

    requestedEnergy_ = resultantEnergy();
    performValidation();

    connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
    connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged(int)));

    connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
    connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged(int)));
}

SGMEnergyPosition::SGMEnergyPosition(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation)
{
    gratingTranslationOptimizationMode_ = ManualMode;
    isUndulatorTracking_ = true;
    isExitSlitPositionTracking_ = true;
    autoDetectUndulatorHarmonic_ = true;
    undulatorOffset_ = 0;

    requestEnergy(requestedEnergy, gratingTranslation);

    performValidation();

    connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
    connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged(int)));

    connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
    connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged(int)));
}

SGMEnergyPosition::SGMEnergyPosition(double requestedEnergy, SGMEnergyPosition::GratingTranslationOptimizationMode gratingOptimizationMode)
{
    gratingTranslationOptimizationMode_ = gratingOptimizationMode;
    isUndulatorTracking_ = true;
    isExitSlitPositionTracking_ = true;
    autoDetectUndulatorHarmonic_ = true;
    undulatorOffset_ = 0;

    requestEnergy(requestedEnergy);

    performValidation();

    connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
    connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged(int)));

    connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
    connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged(int)));
}


SGMEnergyPosition* SGMEnergyPosition::clone() const
{
	SGMEnergyPosition* cloneEnergyPosition = new SGMEnergyPosition(gratingTranslation_,
																   gratingAngle_,
																   undulatorHarmonic_,
																   undulatorPosition_,
																   undulatorOffset_,
																   exitSlitPosition_,
																   0);

	cloneEnergyPosition->setAutoDetectUndulatorHarmonic(autoDetectUndulatorHarmonic_);
	cloneEnergyPosition->setGratingTranslationOptimizationMode(gratingTranslationOptimizationMode_);
	cloneEnergyPosition->setExitSlitPositionTracking(isExitSlitPositionTracking_);
	cloneEnergyPosition->setUndulatorTracking(isUndulatorTracking_);

	return cloneEnergyPosition;
}

bool SGMEnergyPosition::hasErrors() const
{
    return !errorValidator_.isValid();
}

bool SGMEnergyPosition::hasWarnings() const
{
    return !warningValidator_.isValid();
}

QString SGMEnergyPosition::errorMessage() const
{
    return errorValidator_.fullFailureMessage();
}

QString SGMEnergyPosition::warningMessage() const
{
    return warningValidator_.fullFailureMessage();
}

double SGMEnergyPosition::resultantEnergy() const
{
    return energyFromGrating(gratingTranslation_, gratingAngle_);
}

SGMGratingSupport::GratingTranslation SGMEnergyPosition::gratingTranslation() const
{
    return gratingTranslation_;
}


SGMEnergyPosition::GratingTranslationOptimizationMode SGMEnergyPosition::gratingTranslationOptimizationMode() const
{
    return gratingTranslationOptimizationMode_;
}

double SGMEnergyPosition::gratingAngle() const
{
    return gratingAngle_;
}

bool SGMEnergyPosition::isUndulatorTracking() const
{
    return isUndulatorTracking_;
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyPosition::undulatorHarmonic() const
{
    return undulatorHarmonic_;
}

bool SGMEnergyPosition::autoDetectUndulatorHarmonic() const
{
    return autoDetectUndulatorHarmonic_;
}

double SGMEnergyPosition::undulatorPosition() const
{
    return undulatorPosition_;
}

double SGMEnergyPosition::undulatorOffset() const
{
    return undulatorOffset_;
}

bool SGMEnergyPosition::isExitSlitPositionTracking() const
{
    return isExitSlitPositionTracking_;
}

double SGMEnergyPosition::exitSlitPosition() const
{
    return exitSlitPosition_;
}

void SGMEnergyPosition::setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation)
{
	if(gratingTranslation_ != gratingTranslation &&
			gratingTranslationOptimizationMode_ == ManualMode) {

		gratingTranslation_ = gratingTranslation;
		if(autoDetectUndulatorHarmonic_) {
			setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
		}
		optimizeForEnergy();
		performValidation();
		emit gratingTranslationChanged(gratingTranslation_);
		emit energyChanged(resultantEnergy());
	}
}

void SGMEnergyPosition::setGratingTranslationOptimizationMode(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
	if(gratingTranslationOptimizationMode_ != gratingTranslationOptimizationMode) {

		gratingTranslationOptimizationMode_ = gratingTranslationOptimizationMode;
		emit gratingTranslationOptimizationModeChanged(gratingTranslationOptimizationMode_);

		// Should changing this mode cause the components to reposition?
	}
}

void SGMEnergyPosition::setGratingAngle(double gratingAngle)
{
	if(gratingAngle_ != gratingAngle) {
		gratingAngle_ = gratingAngle;
		if(autoDetectUndulatorHarmonic_) {
			setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
		}
		optimizeForEnergy();
		performValidation();
		emit gratingAngleChanged(gratingAngle_);
		emit energyChanged(resultantEnergy());
	}
}

void SGMEnergyPosition::setUndulatorTracking(bool isTracking)
{
	if(isUndulatorTracking_ != isTracking) {

		isUndulatorTracking_ = isTracking;

		// If we've just turned on tracking make sure we're at the right position
		// for the current energy.
		if(isUndulatorTracking_) {
			setUndulatorPosition(optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
		}

		performValidation();
		emit undulatorTrackingChanged(isUndulatorTracking_);
	}
}

void SGMEnergyPosition::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(undulatorHarmonic_ != undulatorHarmonic) {
		undulatorHarmonic_ = undulatorHarmonic;
		setUndulatorPosition(optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
		performValidation();
		emit undulatorHarmonicChanged(undulatorHarmonic_);
	}
}

void SGMEnergyPosition::setAutoDetectUndulatorHarmonic(bool autoDetect)
{
	if(autoDetectUndulatorHarmonic_ != autoDetect) {

		autoDetectUndulatorHarmonic_ = autoDetect;

		if(autoDetectUndulatorHarmonic_) {

			setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
		} else {

			performValidation();
		}
	}
}

void SGMEnergyPosition::setUndulatorPosition(double undulatorPosition)
{
	if(undulatorPosition_ != undulatorPosition) {
		undulatorPosition_ = undulatorPosition;
		performValidation();
		emit undulatorPositionChanged(undulatorPosition);
	}
}

void SGMEnergyPosition::setUndulatorOffset(double undulatorOffset)
{
	if(undulatorOffset_ != undulatorOffset) {
		undulatorOffset_ = undulatorOffset;
		// Need to re-optimize the undulator taking into account the new offset
		setUndulatorPosition(optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
		emit undulatorOffsetChanged(undulatorOffset_);
	}
}

void SGMEnergyPosition::setExitSlitPositionTracking(bool isTracking)
{
	if(isExitSlitPositionTracking_ != isTracking) {

		isExitSlitPositionTracking_ = isTracking;

		// If we've just turned on tracking make sure we're at the right position
		// for the current energy.
		if(isExitSlitPositionTracking_) {
			setExitSlitPosition(optimizedExitSlitPosition(gratingTranslation_, resultantEnergy()));
		}

		performValidation();
		emit exitSlitTrackingChanged(isExitSlitPositionTracking_);
	}
}

void SGMEnergyPosition::setExitSlitPosition(double exitSlitPosition)
{
    if(exitSlitPosition_ != exitSlitPosition) {
        exitSlitPosition_ = exitSlitPosition;
        performValidation();
        emit exitSlitPositionChanged(exitSlitPosition_);
    }
}

void SGMEnergyPosition::requestEnergy(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation)
{
    requestedEnergy_ = requestedEnergy;

    if(gratingTranslationOptimizationMode_ == ManualMode) {
        setGratingTranslation(gratingTranslation);
    }

    if(autoDetectUndulatorHarmonic_) {

        setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation, requestedEnergy));
    }

    setGratingAngle(gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
}

void SGMEnergyPosition::requestEnergy(double requestedEnergy)
{
    SGMGratingSupport::GratingTranslation newGratingTranslation = optimizedGrating(requestedEnergy);

    // Manually set the grating translation
    if(gratingTranslation_ != newGratingTranslation) {

        gratingTranslation_ = newGratingTranslation;
        emit gratingTranslationChanged(gratingTranslation_);
    }

    if(autoDetectUndulatorHarmonic_) {

        SGMUndulatorSupport::UndulatorHarmonic newUndulatorHarmonic = optimizedUndulatorHarmonic(gratingTranslation_, requestedEnergy_);

        if(undulatorHarmonic_ != newUndulatorHarmonic) {

            undulatorHarmonic_ = newUndulatorHarmonic;
            emit undulatorHarmonic_;
        }
    }

    setGratingAngle(gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
}

SGMGratingSupport::GratingTranslation SGMEnergyPosition::optimizedGrating(double requestedEnergy) const
{
    // The grating which produces the largest flux at a given photon energy.
    if(gratingTranslationOptimizationMode_ == OptimizeFlux) {

        if(requestedEnergy < 700) {

            return SGMGratingSupport::LowGrating;
        } else if(requestedEnergy < 1300) {

            return SGMGratingSupport::MediumGrating;
        } else {

            return SGMGratingSupport::HighGrating;
        }
    } else if(gratingTranslationOptimizationMode_ == OptimizeResolution) {

        if (requestedEnergy > 650) {

            return SGMGratingSupport::HighGrating;
        } else if (requestedEnergy > 450) {

            return SGMGratingSupport::MediumGrating;
        } else {

            return SGMGratingSupport::LowGrating;
        }
    } else /*Manual Mode */ {

        return gratingTranslation_;
    }
}

double SGMEnergyPosition::energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double gratingAngleEncoderTarget) const
{
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double curveFitCorrection = SGMGratingSupport::curveFitCorrection(gratingTranslationSelection);
    double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);

    return 1e-9 * 1239.842 / ((2 * gratingSpacing * curveFitCorrection * gratingAngleEncoderTarget) / radiusCurvatureOffset * cos(includedAngle / 2));
}

double SGMEnergyPosition::gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double curveFitCorrection = SGMGratingSupport::curveFitCorrection(gratingTranslationSelection);
    double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);

    return 1e-9 * 1239.842 / ((2 * gratingSpacing * curveFitCorrection * energy) / radiusCurvatureOffset * cos(includedAngle / 2));
}

void SGMEnergyPosition::optimizeForEnergy()
{
    double currentGratingEnergy = resultantEnergy();

    if(isUndulatorTracking_) {
        setUndulatorPosition(optimizedUndulatorPosition(currentGratingEnergy, undulatorHarmonic_, undulatorOffset_));
    }

    if(isExitSlitPositionTracking_) {
        setExitSlitPosition(optimizedExitSlitPosition(gratingTranslation_, currentGratingEnergy));
    }
}

double SGMEnergyPosition::optimizedUndulatorPosition(double energy, SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic, double undulatorOffset) const
{
    return (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;
}

double SGMEnergyPosition::optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);
	double gratingEncoderOffset = 4600; // Distance between the grating and the zero position of the encoder
    double wavelength = (1239.842 / energy) * 1.0e-9;
    double angleOfIncidence = asin((wavelength / (2*gratingSpacing)) / cos(includedAngle/2)) + (includedAngle/2);
    double angleOfDefraction = -includedAngle + angleOfIncidence;

    return -gratingEncoderOffset + (pow(cos(angleOfDefraction),2)) / (((cos(angleOfDefraction + includedAngle) + cos(angleOfDefraction)) / 70480) - (pow(cos(angleOfDefraction + includedAngle),2))/1500);
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyPosition::optimizedUndulatorHarmonic(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    if(gratingTranslationSelection == SGMGratingSupport::HighGrating &&
            energy >= 1400) {

        return SGMUndulatorSupport::ThirdHarmonic;
    } else {

        return SGMUndulatorSupport::FirstHarmonic;
    }
}

void SGMEnergyPosition::performValidation()
{
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_UNDULATOR_HARMONIC, undulatorHarmonic_ == SGMUndulatorSupport::UnknownUndulatorHarmonic);
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_GRATING_TRANSLATION, gratingTranslation_ == SGMGratingSupport::UnknownGrating);
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_HARMONIC, !SGMUndulatorSupport::validEnergy(undulatorHarmonic_, requestedEnergy_));
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_GRATING, !SGMGratingSupport::validEnergy(gratingTranslation_, requestedEnergy_));

    warningValidator_.updateValidity(SGMMONO_UNDULATOR_TRACKING_OFF, !isUndulatorTracking_);
	warningValidator_.updateValidity(SGMMONO_EXIT_SLIT_TRACKING_OFF, !isExitSlitPositionTracking_);
}
