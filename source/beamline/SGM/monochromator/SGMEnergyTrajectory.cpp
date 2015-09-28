#include "SGMEnergyTrajectory.h"

SGMEnergyTrajectory::SGMEnergyTrajectory(double startEnergy,
                                         double endEnergy,
                                         double time,
                                         SGMGratingSupport::GratingTranslation gratingTranslation)
{

    startMonoInfo_ = new SGMMonochromatorInfo(startEnergy, gratingTranslation);
    startEnergy_ = startEnergy;
    endMonoInfo_ = new SGMMonochromatorInfo(endEnergy, gratingTranslation);
    endEnergy_ = endEnergy;

    // ensure both undulator harmonics are the same.
    if(startMonoInfo_->undulatorHarmonic() != endMonoInfo_->undulatorHarmonic()) {

        // If there is a conflict, just use the first harmonic.
        startMonoInfo_->setAutoDetectUndulatorHarmonic(false);
        endMonoInfo_->setAutoDetectUndulatorHarmonic(false);

        startMonoInfo_->setUndulatorHarmonic(SGMUndulatorSupport::FirstHarmonic);
        endMonoInfo_->setUndulatorHarmonic(SGMUndulatorSupport::ThirdHarmonic);
    }

    time_ = time;
}

SGMEnergyTrajectory::~SGMEnergyTrajectory()
{
    startMonoInfo_->deleteLater();
    endMonoInfo_->deleteLater();
}

bool SGMEnergyTrajectory::hasErrors() const
{
    return startMonoInfo_->hasErrors() ||
            endMonoInfo_->hasErrors() ||
            time_ <= 0;
}

bool SGMEnergyTrajectory::hasWarnings() const
{
    return startMonoInfo_->hasWarnings() ||
            endMonoInfo_->hasWarnings();
}

QString SGMEnergyTrajectory::errorMessage() const
{
    QString infoErrorMessage = startMonoInfo_->errorMessage();
    if(startMonoInfo_->hasErrors() && endMonoInfo_->hasErrors()) {
        infoErrorMessage.append("\n");
    }

    infoErrorMessage.append(endMonoInfo_->errorMessage());

    if(time_ <= 0) {
        if(!infoErrorMessage.isEmpty() && endMonoInfo_->hasErrors()) {
            infoErrorMessage.append("\n");
        }
        infoErrorMessage.append("Invalid time for trajectory: Must be greater than zero");
    }

    return infoErrorMessage;
}

QString SGMEnergyTrajectory::warningMessage() const
{
    QString infoWarningMessage;
    infoWarningMessage.append(startMonoInfo_->warningMessage());
    if(startMonoInfo_->hasWarnings() && endMonoInfo_->hasErrors()) {
        infoWarningMessage.append("\n");
    }
    infoWarningMessage.append(endMonoInfo_->warningMessage());

    return infoWarningMessage;
}

double SGMEnergyTrajectory::startGratingAngleEncoderStep() const
{
    return startMonoInfo_->gratingAngle();
}

double SGMEnergyTrajectory::endGratingAngleEncoderStep() const
{
    return endMonoInfo_->gratingAngle();
}

double SGMEnergyTrajectory::gratingAngleVelocity() const
{
    return (endGratingAngleEncoderStep() - startGratingAngleEncoderStep()) / time_;
}

double SGMEnergyTrajectory::startUndulatorPosition() const
{
    return startMonoInfo_->undulatorPosition();
}

double SGMEnergyTrajectory::endUndulatorPosition() const
{
    return endMonoInfo_->undulatorPosition();
}

double SGMEnergyTrajectory::undulatorVelocity() const
{
    return (endUndulatorPosition() - startUndulatorPosition()) / time_;
}

SGMGratingSupport::GratingTranslation SGMEnergyTrajectory::gratingTranslation() const
{
    return gratingTranslation_;
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyTrajectory::undulatorHarmonic() const
{
    // Constructor should have done the job of synching harmonics between start & end
    // so we can return either.
    return startMonoInfo_->undulatorHarmonic();
}

double SGMEnergyTrajectory::startExitSlitPosition() const
{
    return startMonoInfo_->exitSlitPosition();
}

double SGMEnergyTrajectory::endExitSlitPosition() const
{
    return endMonoInfo_->exitSlitPosition();
}

double SGMEnergyTrajectory::exitSlitVelocity() const
{
    return (endExitSlitPosition() - startExitSlitPosition()) / time_;
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

QString SGMEnergyTrajectory::toString() const
{
    QString returnString =  QString("Start Grating Angle: %1 End Grating Angle: %2 Grating Angle Velocity: %3 \n")
            .arg(startGratingAngleEncoderStep())
            .arg(endGratingAngleEncoderStep())
            .arg(gratingAngleVelocity());

    returnString.append(QString("Start Undulator Position: %1 End Undulator Position: %2 Undulator Velocity: %3 \n")
                        .arg(startUndulatorPosition())
                        .arg(endUndulatorPosition())
                        .arg(undulatorVelocity()));

    returnString.append(QString("Start Exit Slit Position: %1 End Exit Slit Position: %2 Exit Slit Velocity: %3\n")
                        .arg(startExitSlitPosition())
                        .arg(endExitSlitPosition())
                        .arg(exitSlitVelocity()));

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
