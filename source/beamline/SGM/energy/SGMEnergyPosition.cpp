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
	errorValidator_ = new AMValidator(this);
	warningValidator_ = new AMValidator(this);

	gratingTranslation_ = gratingTranslation;
	gratingTranslationOptimizationMode_ = SGMGratingSupport::ManualMode;
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
}

SGMEnergyPosition::SGMEnergyPosition(double requestedEnergy,
                                     SGMGratingSupport::GratingTranslation gratingTranslation,
                                     QObject* parent) :
    QObject(parent)
{
	errorValidator_ = new AMValidator(this);
	warningValidator_ = new AMValidator(this);

	gratingTranslationOptimizationMode_ = SGMGratingSupport::ManualMode;
	isUndulatorTracking_ = true;
	isExitSlitPositionTracking_ = true;
	autoDetectUndulatorHarmonic_ = true;
	undulatorOffset_ = 0;

	requestEnergy(requestedEnergy, gratingTranslation);

	performValidation();
}

SGMEnergyPosition::SGMEnergyPosition(double requestedEnergy,
				     SGMGratingSupport::GratingTranslationOptimizationMode gratingOptimizationMode,
                                     QObject* parent) :
    QObject(parent)
{
	gratingTranslationOptimizationMode_ = gratingOptimizationMode;
	isUndulatorTracking_ = true;
	isExitSlitPositionTracking_ = true;
	autoDetectUndulatorHarmonic_ = true;
	undulatorOffset_ = 0;

	requestEnergy(requestedEnergy);

	performValidation();
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

double SGMEnergyPosition::resultantEnergy() const
{
	return SGMGratingSupport::energyFromGrating(gratingTranslation_, gratingAngle_);
}

SGMGratingSupport::GratingTranslation SGMEnergyPosition::gratingTranslation() const
{
	return gratingTranslation_;
}


SGMGratingSupport::GratingTranslationOptimizationMode SGMEnergyPosition::gratingTranslationOptimizationMode() const
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

bool SGMEnergyPosition::hasErrors() const
{
	return !errorValidator_->isValid();
}

bool SGMEnergyPosition::hasWarnings() const
{
	return !warningValidator_->isValid();
}

AMValidator * SGMEnergyPosition::errorValidator() const
{
	return errorValidator_;
}

AMValidator * SGMEnergyPosition::warningValidator() const
{
	return warningValidator_;
}

void SGMEnergyPosition::setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation)
{
	if(gratingTranslation_ != gratingTranslation &&
		gratingTranslationOptimizationMode_ == SGMGratingSupport::ManualMode) {

		gratingTranslation_ = gratingTranslation;
		if(autoDetectUndulatorHarmonic_) {
			setUndulatorHarmonic(SGMUndulatorSupport::optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
		}
		optimizeForEnergy();
		performValidation();
		emit gratingTranslationChanged(gratingTranslation_);
		emit energyChanged(resultantEnergy());
	}
}

void SGMEnergyPosition::setGratingTranslationOptimizationMode(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
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
			setUndulatorHarmonic(SGMUndulatorSupport::optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
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
			setUndulatorPosition(SGMUndulatorSupport::optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
		}

		performValidation();
		emit undulatorTrackingChanged(isUndulatorTracking_);
	}
}

void SGMEnergyPosition::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(undulatorHarmonic_ != undulatorHarmonic) {
		undulatorHarmonic_ = undulatorHarmonic;
		setUndulatorPosition(SGMUndulatorSupport::optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
		performValidation();
		emit undulatorHarmonicChanged(undulatorHarmonic_);
	}
}

void SGMEnergyPosition::setAutoDetectUndulatorHarmonic(bool autoDetect)
{
	if(autoDetectUndulatorHarmonic_ != autoDetect) {

		autoDetectUndulatorHarmonic_ = autoDetect;

		if(autoDetectUndulatorHarmonic_) {

			setUndulatorHarmonic(SGMUndulatorSupport::optimizedUndulatorHarmonic(gratingTranslation_, resultantEnergy()));
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
		setUndulatorPosition(SGMUndulatorSupport::optimizedUndulatorPosition(resultantEnergy(), undulatorHarmonic_, undulatorOffset_));
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
			setExitSlitPosition(SGMExitSlitSupport::optimizedExitSlitPosition(gratingTranslation_, resultantEnergy()));
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

	if(gratingTranslationOptimizationMode_ == SGMGratingSupport::ManualMode) {
		setGratingTranslation(gratingTranslation);
	}

	if(autoDetectUndulatorHarmonic_) {

		setUndulatorHarmonic(SGMUndulatorSupport::optimizedUndulatorHarmonic(gratingTranslation, requestedEnergy));
	}

	setGratingAngle(SGMGratingSupport::gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
}

void SGMEnergyPosition::requestEnergy(double requestedEnergy)
{
	requestedEnergy_ = requestedEnergy;

	if(gratingTranslationOptimizationMode_ != SGMGratingSupport::ManualMode) {
		SGMGratingSupport::GratingTranslation newGratingTranslation = SGMGratingSupport::optimizedGratingTranslation(requestedEnergy, gratingTranslationOptimizationMode_);

		// Set the new grating translation if it's different.
		if(gratingTranslation_ != newGratingTranslation) {

			gratingTranslation_ = newGratingTranslation;
			emit gratingTranslationChanged(gratingTranslation_);
		}
	}

	if(autoDetectUndulatorHarmonic_) {

		SGMUndulatorSupport::UndulatorHarmonic newUndulatorHarmonic = SGMUndulatorSupport::optimizedUndulatorHarmonic(gratingTranslation_, requestedEnergy_);

		if(undulatorHarmonic_ != newUndulatorHarmonic) {

			undulatorHarmonic_ = newUndulatorHarmonic;
			emit undulatorHarmonicChanged(undulatorHarmonic_);
		}
	}

	setGratingAngle(SGMGratingSupport::gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
}

void SGMEnergyPosition::optimizeForEnergy()
{
	double currentGratingEnergy = resultantEnergy();

	if(isUndulatorTracking_) {
		setUndulatorPosition(SGMUndulatorSupport::optimizedUndulatorPosition(currentGratingEnergy, undulatorHarmonic_, undulatorOffset_));
	}

	if(isExitSlitPositionTracking_) {
		setExitSlitPosition(SGMExitSlitSupport::optimizedExitSlitPosition(gratingTranslation_, currentGratingEnergy));
	}
}

void SGMEnergyPosition::performValidation()
{
	errorValidator_->updateValidity(SGMENERGY_UNKNOWN_UNDULATOR_HARMONIC, undulatorHarmonic_ == SGMUndulatorSupport::UnknownUndulatorHarmonic);
	errorValidator_->updateValidity(SGMENERGY_UNKNOWN_GRATING_TRANSLATION, gratingTranslation_ == SGMGratingSupport::UnknownGrating);
	errorValidator_->updateValidity(SGMENERGY_INVALID_ENERGY_FOR_GRATING, gratingAngle_ < -556800);
	warningValidator_->updateValidity(SGMENERGY_UNDULATOR_TRACKING_OFF, !isUndulatorTracking_);
	warningValidator_->updateValidity(SGMENERGY_EXIT_SLIT_TRACKING_OFF, !isExitSlitPositionTracking_);
}
