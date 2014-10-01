#include "BioXASSideMonoCrystalChangeView.h"

BioXASSideMonoCrystalChangeView::BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent) :
    QWidget(parent)
{
    steps_ = new BioXASSideMonoCrystalChangeSteps(toView, this);
}

BioXASSideMonoCrystalChangeView::~BioXASSideMonoCrystalChangeView()
{

}

void BioXASSideMonoCrystalChangeView::onStepChanged()
{

}






BioXASSideMonoCrystalChangeSteps::BioXASSideMonoCrystalChangeSteps(BioXASSideMonochromator* mono, QObject *parent) :
    QObject(parent)
{
    currentStep_ = BeforeBrakeOn;
    setMono(mono);
}

BioXASSideMonoCrystalChangeSteps::~BioXASSideMonoCrystalChangeSteps()
{

}

BioXASSideMonoCrystalChangeSteps::Step BioXASSideMonoCrystalChangeSteps::currentStep() const
{
    return currentStep_;
}

BioXASSideMonochromator* BioXASSideMonoCrystalChangeSteps::mono() const
{
    return mono_;
}

void BioXASSideMonoCrystalChangeSteps::reset()
{
    setNextStep(BeforeBrakeOn);
}

void BioXASSideMonoCrystalChangeSteps::setMono(BioXASSideMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_) {
            disconnect( mono_, 0, this, 0 );
        }

        mono_ = newMono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );
            connect( mono_, SIGNAL(brakeOffStatusChanged(bool)), this, SLOT(onMonoBrakeStatusChanged()) );
            connect( mono_, SIGNAL(crystalChangePositionStatusChanged(bool)), this, SLOT(onMonoPositionStatusChanged()) );
            connect( mono_, SIGNAL(keyStatusChanged(bool)), this, SLOT(onMonoKeyStatusChanged()) );
        }
    }
}

void BioXASSideMonoCrystalChangeSteps::setNextStep(Step nextStep)
{
    if (nextStep == BeforeBrakeOn) {
        currentStep_ = BeforeBrakeOn;

        onMonoBrakeStatusChanged();
        onMonoPositionStatusChanged();
        onMonoKeyStatusChanged();

    } else if (currentStep_ == BeforeBrakeOn && nextStep == BrakeOn) {
        currentStep_ = BrakeOn;

    } else if (currentStep_ == BrakeOn && nextStep == AtCrystalChangePosition) {
        currentStep_ = AtCrystalChangePosition;

    } else if (currentStep_ == AtCrystalChangePosition && nextStep == KeyTurned) {
        currentStep_ = KeyTurned;

    } else if (currentStep_ == KeyTurned && nextStep == BrakeOff) {
        currentStep_ = Complete;
    }

    emit stepChanged(currentStep_);
}

void BioXASSideMonoCrystalChangeSteps::onMonoBrakeStatusChanged()
{
    if (mono_ && mono_->isConnected()) {
        if (mono_->brakeOff()) {
            setNextStep(BrakeOff);

        } else {
            setNextStep(BrakeOn);
        }
    }
}

void BioXASSideMonoCrystalChangeSteps::onMonoPositionStatusChanged()
{
    if (mono_ && mono_->isConnected()) {
        if (mono_->atCrystalChangePosition()) {
            setNextStep(AtCrystalChangePosition);
        }
    }
}

void BioXASSideMonoCrystalChangeSteps::onMonoKeyStatusChanged()
{
    if (mono_ && mono_->isConnected()) {
        if (mono_->keyStatus()) {
            setNextStep(KeyTurned);
        }
    }
}
