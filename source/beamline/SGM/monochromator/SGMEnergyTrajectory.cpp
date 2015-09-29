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
        endMonoInfo_->setUndulatorHarmonic(SGMUndulatorSupport::FirstHarmonic);
    }

    time_ = time;

    /*
     * The velocity profile class needs to be refines, as per Issue 1563.
     * Some noted refinements:
     *   - Should the general class be a motion profile rather than a velocity profile?
     *   - Is there a need for a general class system for these (AMVelocityProfile/AMMotionProfile)?
     *   - Do we need time based information (velocityProfile_.velocityAt(double time);)?
     * My suspicion is that the answers will come into focus while working on
     * Issue 1566.
     *   - Also need to figure out what the acceleration values are for the controls
     *     on the beamline.
     */
    gratingAngleVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, (endGratingAngleEncoderStep() - startGratingAngleEncoderStep()) / time_, -5.0);
    undulatorVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, (endUndulatorPosition() - startUndulatorPosition()) / time_, -5.0);
    exitSlitVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, (endExitSlitPosition() - startExitSlitPosition()) / time_, -5.0);
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

AMTrapezoidVelocityProfile SGMEnergyTrajectory::gratingAngleVelocityProfile() const
{
    return gratingAngleVelocityProfile_;
}

double SGMEnergyTrajectory::startUndulatorPosition() const
{
    return startMonoInfo_->undulatorPosition();
}

double SGMEnergyTrajectory::endUndulatorPosition() const
{
    return endMonoInfo_->undulatorPosition();
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

QString SGMEnergyTrajectory::toString() const
{
    QString returnString =  QString("Start Grating Angle: %1 End Grating Angle: %2 Grating Angle Velocity: %3 \n")
            .arg(startGratingAngleEncoderStep())
            .arg(endGratingAngleEncoderStep())
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
