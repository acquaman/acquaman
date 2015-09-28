#include "SGMMonochromatorInfo.h"

#include <cmath>
SGMMonochromatorInfo::SGMMonochromatorInfo(SGMGratingSupport::GratingTranslation gratingTranslation,
                                           double gratingAngle,
                                           SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
                                           double undulatorPosition,
                                           double undulatorOffset,
                                           double exitSlitPosition,
                                           QObject *parent) :
    QObject(parent)
{
    gratingTranslation_ = gratingTranslation;
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

SGMMonochromatorInfo::SGMMonochromatorInfo(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation)
{
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

SGMMonochromatorInfo::SGMMonochromatorInfo(double requestedEnergy, SGMMonochromatorInfo::GratingTranslationOptimizationMode gratingOptimizationMode)
{
    isUndulatorTracking_ = true;
    isExitSlitPositionTracking_ = true;
    autoDetectUndulatorHarmonic_ = true;
    undulatorOffset_ = 0;

    requestEnergy(requestedEnergy, gratingOptimizationMode);

    performValidation();

    connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
    connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged(int)));

    connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
    connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged(int)));
}

bool SGMMonochromatorInfo::hasErrors() const
{
    return !errorValidator_.isValid();
}

bool SGMMonochromatorInfo::hasWarnings() const
{
    return !warningValidator_.isValid();
}

QString SGMMonochromatorInfo::errorMessage() const
{
    return errorValidator_.fullFailureMessage();
}

QString SGMMonochromatorInfo::warningMessage() const
{
    return warningValidator_.fullFailureMessage();
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
        if(autoDetectUndulatorHarmonic_) {
            setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
        }
        optimizeForEnergy();
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
        if(autoDetectUndulatorHarmonic_) {
            setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
        }
        optimizeForEnergy();
        performValidation();
        emit gratingAngleChanged(gratingAngle_);
        emit energyChanged(resultantEnergy());
    }
}

bool SGMMonochromatorInfo::isUndulatorTracking() const
{
    return isUndulatorTracking_;
}

void SGMMonochromatorInfo::setUndulatorTracking(bool isTracking)
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

SGMUndulatorSupport::UndulatorHarmonic SGMMonochromatorInfo::undulatorHarmonic() const
{
    return undulatorHarmonic_;
}

void SGMMonochromatorInfo::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
    if(undulatorHarmonic_ != undulatorHarmonic) {
        undulatorHarmonic_ = undulatorHarmonic;
        setUndulatorPosition(optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
        performValidation();
        emit undulatorHarmonicChanged(undulatorHarmonic_);
    }
}

bool SGMMonochromatorInfo::autoDetectUndulatorHarmonic() const
{
    return autoDetectUndulatorHarmonic_;
}

void SGMMonochromatorInfo::setAutoDetectUndulatorHarmonic(bool autoDetect)
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
        // Need to re-optimize the undulator taking into account the new offset
        setUndulatorPosition(optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
        emit undulatorOffsetChanged(undulatorOffset_);
    }
}

bool SGMMonochromatorInfo::isExitSlitPositionTracking() const
{
    return isExitSlitPositionTracking_;
}

void SGMMonochromatorInfo::setExitSlitPositionTracking(bool isTracking)
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

    if(autoDetectUndulatorHarmonic_) {

        setUndulatorHarmonic(optimizedUndulatorHarmonic(gratingTranslation, requestedEnergy));
    }

    setGratingAngle(gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
}

void SGMMonochromatorInfo::requestEnergy(double requestedEnergy, GratingTranslationOptimizationMode optimizationMode)
{
    requestEnergy(requestedEnergy, optimizedGrating(requestedEnergy, optimizationMode));
}

SGMGratingSupport::GratingTranslation SGMMonochromatorInfo::optimizedGrating(double requestedEnergy, SGMMonochromatorInfo::GratingTranslationOptimizationMode optimizationMode) const
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
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double curveFitCorrection = SGMGratingSupport::curveFitCorrection(gratingTranslationSelection);
    double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);

    return 1e-9 * 1239.842 / ((2 * gratingSpacing * curveFitCorrection * gratingAngleEncoderTarget) / radiusCurvatureOffset * cos(includedAngle / 2));
}

double SGMMonochromatorInfo::gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double curveFitCorrection = SGMGratingSupport::curveFitCorrection(gratingTranslationSelection);
    double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);

    return 1e-9 * 1239.842 / ((2 * gratingSpacing * curveFitCorrection * energy) / radiusCurvatureOffset * cos(includedAngle / 2));
}

void SGMMonochromatorInfo::optimizeForEnergy()
{
    double currentGratingEnergy = resultantEnergy();

    if(isUndulatorTracking_) {
        setUndulatorPosition(optimizedUndulatorPosition(currentGratingEnergy, undulatorHarmonic_, undulatorOffset_));
    }

    if(isExitSlitPositionTracking_) {
        setExitSlitPosition(optimizedExitSlitPosition(gratingTranslation_, currentGratingEnergy));
    }
}

double SGMMonochromatorInfo::optimizedUndulatorPosition(double energy, SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic, double undulatorOffset) const
{
    return (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;
}

double SGMMonochromatorInfo::optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    double gratingSpacing = SGMGratingSupport::gratingSpacing(gratingTranslationSelection);
    double includedAngle = SGMGratingSupport::includedAngle(gratingTranslationSelection);
    double gratingEncoderOffset = 4546; // Distance between the grating and the zero position of the encoder
    double wavelength = (1239.842 / energy) * 1.0e-9;
    double angleOfIncidence = asin((wavelength / (2*gratingSpacing)) / cos(includedAngle/2)) + (includedAngle/2);
    double angleOfDefraction = -includedAngle + angleOfIncidence;

    return -gratingEncoderOffset + (pow(cos(angleOfDefraction),2)) / (((cos(angleOfDefraction + includedAngle) + cos(angleOfDefraction)) / 70480) - (pow(cos(angleOfDefraction + includedAngle),2))/1500);
}

SGMUndulatorSupport::UndulatorHarmonic SGMMonochromatorInfo::optimizedUndulatorHarmonic(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
    if(gratingTranslationSelection == SGMGratingSupport::HighGrating &&
            energy >= 1400) {

        return SGMUndulatorSupport::ThirdHarmonic;
    } else {

        return SGMUndulatorSupport::FirstHarmonic;
    }
}

void SGMMonochromatorInfo::performValidation()
{
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_UNDULATOR_HARMONIC, undulatorHarmonic_ == SGMUndulatorSupport::UnknownUndulatorHarmonic);
    errorValidator_.updateValidity(SGMMONO_UNKNOWN_GRATING_TRANSLATION, gratingTranslation_ == SGMGratingSupport::UnknownGrating);
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_HARMONIC, !SGMUndulatorSupport::validEnergy(undulatorHarmonic_, requestedEnergy_));
    errorValidator_.updateValidity(SGMMONO_INVALID_ENERGY_FOR_GRATING, !SGMGratingSupport::validEnergy(gratingTranslation_, requestedEnergy_));

    warningValidator_.updateValidity(SGMMONO_UNDULATOR_TRACKING_OFF, !isUndulatorTracking_);
    warningValidator_.updateValidity(SGMMONO_EXIT_SLIT_TRACKING_OFF, !isExitSlitPositionTracking_);
}
