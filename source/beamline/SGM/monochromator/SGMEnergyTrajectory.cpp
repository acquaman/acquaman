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
