#include "SGMEnergyTrajectory.h"

SGMEnergyTrajectory::SGMEnergyTrajectory(double startEnergy,
                                         double endEnergy,
                                         double time,
                                         SGMGratingSupport::GratingTranslation gratingTranslation)
{

    startEnergyPosition_ = new SGMEnergyPosition(startEnergy, gratingTranslation);
    startEnergy_ = startEnergy;
    endEnergyPosition_ = new SGMEnergyPosition(endEnergy, gratingTranslation);
    endEnergy_ = endEnergy;

    // ensure both undulator harmonics are the same.
    if(startEnergyPosition_->undulatorHarmonic() != endEnergyPosition_->undulatorHarmonic()) {

        // If there is a conflict, just use the first harmonic.
        startEnergyPosition_->setAutoDetectUndulatorHarmonic(false);
        endEnergyPosition_->setAutoDetectUndulatorHarmonic(false);

        startEnergyPosition_->setUndulatorHarmonic(SGMUndulatorSupport::FirstHarmonic);
        endEnergyPosition_->setUndulatorHarmonic(SGMUndulatorSupport::FirstHarmonic);
    }

    time_ = time;

    /*
	 * The velocity profile class needs to be refined, as per Issue 1563.
     * Some noted refinements:
     *   - Should the general class be a motion profile rather than a velocity profile?
     *   - Is there a need for a general class system for these (AMVelocityProfile/AMMotionProfile)?
     *   - Do we need time based information (velocityProfile_.velocityAt(double time);)?
     * My suspicion is that the answers will come into focus while working on
     * Issue 1566.
     *   - Also need to figure out what the acceleration values are for the controls
     *     on the beamline.
     */
	gratingAngleVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, qAbs(endGratingAngleEncoderStep() - startGratingAngleEncoderStep()) / time_, -5.0);
	undulatorVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, qAbs(endUndulatorPosition() - startUndulatorPosition()) / time_, -5.0);
	exitSlitVelocityProfile_ = AMTrapezoidVelocityProfile(0, 5.0, qAbs(endExitSlitPosition() - startExitSlitPosition()) / time_, -5.0);
}

SGMEnergyTrajectory::~SGMEnergyTrajectory()
{
    startEnergyPosition_->deleteLater();
    endEnergyPosition_->deleteLater();
}

bool SGMEnergyTrajectory::hasErrors() const
{
    return startEnergyPosition_->hasErrors() ||
            endEnergyPosition_->hasErrors() ||
            time_ <= 0;
}

bool SGMEnergyTrajectory::hasWarnings() const
{
    return startEnergyPosition_->hasWarnings() ||
            endEnergyPosition_->hasWarnings();
}

QString SGMEnergyTrajectory::errorMessage() const
{
    QString infoErrorMessage = startEnergyPosition_->errorMessage();
    if(startEnergyPosition_->hasErrors() && endEnergyPosition_->hasErrors()) {
        infoErrorMessage.append("\n");
    }

    infoErrorMessage.append(endEnergyPosition_->errorMessage());

    if(time_ <= 0) {
        if(!infoErrorMessage.isEmpty() && endEnergyPosition_->hasErrors()) {
            infoErrorMessage.append("\n");
        }
        infoErrorMessage.append("Invalid time for trajectory: Must be greater than zero");
    }

    return infoErrorMessage;
}

QString SGMEnergyTrajectory::warningMessage() const
{
    QString infoWarningMessage;
    infoWarningMessage.append(startEnergyPosition_->warningMessage());
    if(startEnergyPosition_->hasWarnings() && endEnergyPosition_->hasErrors()) {
        infoWarningMessage.append("\n");
    }
    infoWarningMessage.append(endEnergyPosition_->warningMessage());

    return infoWarningMessage;
}

double SGMEnergyTrajectory::startGratingAngleEncoderStep() const
{
    return startEnergyPosition_->gratingAngle();
}

double SGMEnergyTrajectory::endGratingAngleEncoderStep() const
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
