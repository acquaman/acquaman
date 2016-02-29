#include "SGMEnergyTrajectory.h"
#include <math.h>

SGMEnergyTrajectory::SGMEnergyTrajectory(double startEnergy,
                                         double endEnergy,
                                         double time,
                                         SGMGratingSupport::GratingTranslation gratingTranslation,
                                         double gratingAngleAcceleration,
                                         double gratingAngleStepsPerEncoderCount,
                                         double undulatorAcceleration,
                                         double undulatorCurrentGap,
                                         double undulatorCurrentStep,
                                         SGMUndulatorSupport::UndulatorHarmonic currentHarmonic)
{

	startEnergyPosition_ = new SGMEnergyPosition(startEnergy, gratingTranslation);
	startEnergy_ = startEnergy;
	endEnergyPosition_ = new SGMEnergyPosition(endEnergy, gratingTranslation);
	endEnergy_ = endEnergy;

	errorValidator_ = new AMValidator();
	errorValidator_->addChildValidator(startEnergyPosition_->errorValidator());
	errorValidator_->addChildValidator(endEnergyPosition_->errorValidator());

	warningValidator_ = new AMValidator();
	warningValidator_->addChildValidator(startEnergyPosition_->warningValidator());
	warningValidator_->addChildValidator(endEnergyPosition_->warningValidator());

	startEnergyPosition_->setAutoDetectUndulatorHarmonic(false);
	endEnergyPosition_->setAutoDetectUndulatorHarmonic(false);

	startEnergyPosition_->setUndulatorHarmonic(currentHarmonic);
	endEnergyPosition_->setUndulatorHarmonic(currentHarmonic);

	time_ = time;

	gratingAngleVelocityProfile_ = AMTrapezoidVelocityProfile(startEnergyPosition_->gratingAngle() * gratingAngleStepsPerEncoderCount,
	                                                          endEnergyPosition_->gratingAngle() * gratingAngleStepsPerEncoderCount,
	                                                          gratingAngleAcceleration,
	                                                          time);

	double startUndulatorSteps = SGMUndulatorSupport::undulatorStepFromPosition(startEnergyPosition_->undulatorPosition(),
	                                                                            undulatorCurrentGap,
	                                                                            undulatorCurrentStep);

	double endUndulatorSteps = SGMUndulatorSupport::undulatorStepFromPosition(endEnergyPosition_->undulatorPosition(),
	                                                                          undulatorCurrentGap,
	                                                                          undulatorCurrentStep);

	undulatorVelocityProfile_ = AMTrapezoidVelocityProfile(startUndulatorSteps,
	                                                       endUndulatorSteps,
	                                                       undulatorAcceleration,
	                                                       time);

	exitSlitVelocityProfile_ = AMTrapezoidVelocityProfile(startEnergyPosition_->exitSlitPosition(),
	                                                      endEnergyPosition_->exitSlitPosition(),
	                                                      5000,
	                                                      time);

}

SGMEnergyTrajectory::~SGMEnergyTrajectory()
{
	startEnergyPosition_->deleteLater();
	endEnergyPosition_->deleteLater();
	errorValidator_->deleteLater();
	warningValidator_->deleteLater();
}

bool SGMEnergyTrajectory::hasErrors() const
{
	return !errorValidator_->isValid();
}

bool SGMEnergyTrajectory::hasWarnings() const
{
	return !warningValidator_->isValid();
}

AMValidator * SGMEnergyTrajectory::errorValidator() const
{
	return errorValidator_;
}

AMValidator * SGMEnergyTrajectory::warningValidator() const
{
	return warningValidator_;
}

double SGMEnergyTrajectory::startGratingAngleEncoderCount() const
{
	return startEnergyPosition_->gratingAngle();
}

double SGMEnergyTrajectory::endGratingAngleEncoderCount() const
{
	return endEnergyPosition_->gratingAngle();
}

AMTrapezoidVelocityProfile SGMEnergyTrajectory::gratingAngleVelocityProfile() const
{
	return gratingAngleVelocityProfile_;
}

double SGMEnergyTrajectory::startUndulatorPosition() const
{
	return startEnergyPosition_->undulatorPosition();
}

double SGMEnergyTrajectory::endUndulatorPosition() const
{
	return endEnergyPosition_->undulatorPosition();
}

AMTrapezoidVelocityProfile SGMEnergyTrajectory::undulatorVelocityProfile() const
{
	return undulatorVelocityProfile_;
}

SGMGratingSupport::GratingTranslation SGMEnergyTrajectory::gratingTranslation() const
{
	return gratingTranslation_;
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyTrajectory::undulatorHarmonic() const
{
	// Constructor should have done the job of synching harmonics between start & end
	// so we can return either.
	return startEnergyPosition_->undulatorHarmonic();
}

double SGMEnergyTrajectory::startExitSlitPosition() const
{
	return startEnergyPosition_->exitSlitPosition();
}

double SGMEnergyTrajectory::endExitSlitPosition() const
{
	return endEnergyPosition_->exitSlitPosition();
}

AMTrapezoidVelocityProfile SGMEnergyTrajectory::exitSlitVelocityProfile() const
{
	return exitSlitVelocityProfile_;
}

double SGMEnergyTrajectory::time() const
{
	return time_;
}

double SGMEnergyTrajectory::startEnergy() const
{
	return startEnergy_;
}

double SGMEnergyTrajectory::endEnergy() const
{
	return endEnergy_;
}

double SGMEnergyTrajectory::energyVelocity() const
{
	if(time_ != 0) {
		return qAbs(endEnergy_ - startEnergy_) / time_;
	} else {
		return 0;
	}
}

QString SGMEnergyTrajectory::toString() const
{
	QString returnString =  QString("Start Grating Angle: %1 End Grating Angle: %2 Grating Angle Velocity: %3 \n")
	        .arg(startGratingAngleEncoderCount())
	        .arg(endGratingAngleEncoderCount())
	        .arg(gratingAngleVelocityProfile().targetVelocity());

	returnString.append(QString("Start Undulator Position: %1 End Undulator Position: %2 Undulator Velocity: %3 \n")
	                    .arg(startUndulatorPosition())
	                    .arg(endUndulatorPosition())
	                    .arg(undulatorVelocityProfile().targetVelocity()));

	returnString.append(QString("Start Exit Slit Position: %1 End Exit Slit Position: %2 Exit Slit Velocity: %3\n")
	                    .arg(startExitSlitPosition())
	                    .arg(endExitSlitPosition())
	                    .arg(exitSlitVelocityProfile().targetVelocity()));

	QString undulatorHarmonicString;

	switch(undulatorHarmonic()) {
	case SGMUndulatorSupport::FirstHarmonic:
		undulatorHarmonicString = "First Harmonic";
		break;
	case SGMUndulatorSupport::ThirdHarmonic:
		undulatorHarmonicString = "Third Harmonic";
		break;
	default:
		undulatorHarmonicString = "Unknown Harmonic";
		break;
	}

	returnString.append(QString("Undulator Harmonic: %1").arg(undulatorHarmonicString));

	return returnString;
}

void SGMEnergyTrajectory::performValidation()
{
	errorValidator_->updateValidity(SGMENERGYTRAJECTORY_INVALID_TIME, time_ <= 0);
}




