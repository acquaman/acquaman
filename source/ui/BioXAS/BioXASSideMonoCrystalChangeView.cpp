#include "BioXASSideMonoCrystalChangeView.h"

BioXASSideMonoCrystalChangeView::BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = 0;

    closeSlits_ = 0;
    removePaddle_ = 0;
    turnKeyCCW_ = 0;
    toCrystalChangePosition_ = 0;
    disableBrake_ = 0;
    fromCrystalChangePosition_ = 0;
    enableBrake_ = 0;
    toNewRegion_ = 0;
    turnKeyCW_ = 0;

    // Current conditions.

    setMono(toView);
}

BioXASSideMonoCrystalChangeView::~BioXASSideMonoCrystalChangeView()
{

}

BioXASSideMonochromator* BioXASSideMonoCrystalChangeView::mono() const
{
    return mono_;
}

void BioXASSideMonoCrystalChangeView::setMono(BioXASSideMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = newMono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );
        }

        onMonoConnectedChanged();
    }
}

void BioXASSideMonoCrystalChangeView::onMonoConnectedChanged()
{
    if (mono_ && mono_->isConnected()) {
        // if mono is connected...

    } else {
        // if mono is disconnected...
    }
}

void BioXASSideMonoCrystalChangeView::createSteps()
{

}

void BioXASSideMonoCrystalChangeView::createUI()
{

}









BioXASSideMonoCrystalChangeStepView::BioXASSideMonoCrystalChangeStepView(BioXASSideMonoCrystalChangeStep *toView, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    step_ = 0;
    isExtended_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    stepNumber_ = new QLabel(this);
    mainLayout->addWidget(stepNumber_, 0, 0);

    stepBasicInstructions_ = new QLabel(this);
    mainLayout->addWidget(stepBasicInstructions_, 0, 1);

    moreButton_ = new QPushButton("More", this);
    connect( moreButton_, SIGNAL(clicked()), this, SLOT(onMoreButtonClicked()) );
    mainLayout->addWidget(moreButton_, 0, 2);

    stepDetailedInstructions_ = new QLabel(this);
    mainLayout->addWidget(stepDetailedInstructions_, 1, 0, 3, 3);
    stepDetailedInstructions_->hide();

    setLayout(mainLayout);

    // Current settings.

    setStep(toView);
}

BioXASSideMonoCrystalChangeStepView::~BioXASSideMonoCrystalChangeStepView()
{

}

BioXASSideMonoCrystalChangeStep* BioXASSideMonoCrystalChangeStepView::step() const
{
    return step_;
}

bool BioXASSideMonoCrystalChangeStepView::isExtended() const
{
    return isExtended_;
}

void BioXASSideMonoCrystalChangeStepView::setStep(BioXASSideMonoCrystalChangeStep *newStep)
{
    if (step_ != newStep) {

        if (step_)
            disconnect( step_, 0, this, 0 );

        step_ = newStep;

        if (step_) {
            connect( step_, SIGNAL(actionChanged(AMAction3*)), this, SLOT(onStepActionChanged()) );
            connect( step_, SIGNAL(stepNumberChanged(int)), this, SLOT(onStepNumberChanged()) );
            connect( step_, SIGNAL(basicInstructionsChanged(QString)), this, SLOT(onStepBasicInstructionsChanged()) );
            connect( step_, SIGNAL(detailedInstructionsChanged(QString)), this, SLOT(onStepDetailedInstructionsChanged()) );
            connect( step_, SIGNAL(stepStarted()), this, SLOT(onStepStarted()) );
            connect( step_, SIGNAL(stepSucceeded()), this, SLOT(onStepSucceeded()) );
            connect( step_, SIGNAL(stepFailed()), this, SLOT(onStepFailed()) );
        }

        emit stepChanged(step_);
    }
}

void BioXASSideMonoCrystalChangeStepView::setExtended(bool isExtended)
{
    if (isExtended_ != isExtended) {

        isExtended_ = isExtended;

        if (isExtended) {
            stepDetailedInstructions_->show();

        } else {
            stepDetailedInstructions_->hide();
        }

        emit extendedChanged(isExtended_);
    }
}

void BioXASSideMonoCrystalChangeStepView::onMoreButtonClicked()
{
    setExtended(!isExtended_);
}

void BioXASSideMonoCrystalChangeStepView::onStepActionChanged()
{

}

void BioXASSideMonoCrystalChangeStepView::onStepNumberChanged()
{
    if (step_) {
        stepNumber_->setText(QString::number(step_->number()));
    }
}

void BioXASSideMonoCrystalChangeStepView::onStepBasicInstructionsChanged()
{
    if (step_) {
        stepBasicInstructions_->setText(step_->basicInstructions());
    }
}

void BioXASSideMonoCrystalChangeStepView::onStepDetailedInstructionsChanged()
{
    if (step_) {
        stepDetailedInstructions_->setText(step_->detailedInstructions());
    }
}

void BioXASSideMonoCrystalChangeStepView::onStepStarted()
{

}

void BioXASSideMonoCrystalChangeStepView::onStepSucceeded()
{

}

void BioXASSideMonoCrystalChangeStepView::onStepFailed()
{

}









BioXASSideMonoCrystalChangeStep::BioXASSideMonoCrystalChangeStep(AMAction3 *action, QObject *parent) :
    QObject(parent)
{
    // Set variables.

    action_ = 0;
    number_ = 0;
    basicInstructions_ = "";
    detailedInstructions_ = "";

    // Current settings.

    setAction(action);
}

BioXASSideMonoCrystalChangeStep::~BioXASSideMonoCrystalChangeStep()
{

}

AMAction3* BioXASSideMonoCrystalChangeStep::action() const
{
    return action_;
}

int BioXASSideMonoCrystalChangeStep::number() const
{
    return number_;
}

QString BioXASSideMonoCrystalChangeStep::basicInstructions() const
{
    return basicInstructions_;
}

QString BioXASSideMonoCrystalChangeStep::detailedInstructions() const
{
    return detailedInstructions_;
}

void BioXASSideMonoCrystalChangeStep::setAction(AMAction3 *newAction)
{
    if (action_ != newAction) {

        if (action_)
            disconnect( action_, 0, this, 0 );

        action_ = newAction;

        if (action_) {
            connect( action_, SIGNAL(started()), this, SIGNAL(stepStarted()) );
            connect( action_, SIGNAL(succeeded()), this, SIGNAL(stepSucceeded()) );
            connect( action_, SIGNAL(failed()), this, SIGNAL(stepFailed()) );
        }

        emit actionChanged(action_);
    }
}

void BioXASSideMonoCrystalChangeStep::setNumber(int newNumber)
{
    if (number_ != newNumber) {
        number_ = newNumber;
        emit numberChanged(number_);
    }
}

void BioXASSideMonoCrystalChangeStep::setBasicInstructions(const QString &newInstructions)
{
    if (basicInstructions_ != newInstructions) {
        basicInstructions_ = newInstructions;
        emit basicInstructionsChanged(basicInstructions_);
    }
}

void BioXASSideMonoCrystalChangeStep::setDetailedInstructions(const QString &newInstructions)
{
    if (detailedInstructions_ != newInstructions) {
        detailedInstructions_ = newInstructions;
        emit detailedInstructionsChanged(detailedInstructions_);
    }
}



