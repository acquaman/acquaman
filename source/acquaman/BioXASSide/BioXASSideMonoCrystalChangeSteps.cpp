#include "BioXASSideMonoCrystalChangeSteps.h"

BioXASSideMonoCrystalChangeSteps::BioXASSideMonoCrystalChangeSteps(AMAction3 *crystalChangeActions, QObject *parent) :
    QObject(parent)
{
    // Set variables.

    actions_ = 0;
    currentStepIndex_ = -1;

    // Create steps.

    closeSlits_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    closeSlits_->setDescription("Closing slits...");

    removePaddle_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    removePaddle_->setDescription("Removing paddle...");

    enableKey_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    enableKey_->setDescription("Turn KEY to ENABLE, counter-clockwise...");
    enableKey_->setUserInstructions("Walk to the racks and turn the crystal change KEY to ENABLED. This allows a crystal change to take place.");

    moveToCCPosition_ = new BioXASSideMonoCrystalChangeStep(0, "Moving to crystal change position...", "", this);
    moveToCCPosition_->setDescription("Moving stage to 55deg...");

    disableBrake_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    disableBrake_->setDescription("Flip brake SWITCH to DISABLED...");
    disableBrake_->setUserInstructions("Walk to the racks and flip the brake SWITCH to DISABLED. This allows the crystal change motor to move, where normally it wouldn't. The mono must be at 55deg, or horrible things will happen!!");

    switchCrystals_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    switchCrystals_->setDescription("Switching crystals...");

    enableBrake_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    enableBrake_->setDescription("Flip brake SWITCH to ENABLED...");
    enableBrake_->setUserInstructions("Walk to the racks and flip the brake SWITCH to ENABLED. This prevents the crystal change motor from moving during normal mono operation.");

    moveIntoNewRegion_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    moveIntoNewRegion_->setDescription("Moving into new region...");

    disableKey_ = new BioXASSideMonoCrystalChangeStep(0, "", "", this);
    disableKey_->setDescription("Turn KEY to DISABLE, clockwise...");
    disableKey_->setUserInstructions("Walk to the racks and turn the crystal change KEY to DISABLED.");

    steps_.append(closeSlits_);
    steps_.append(removePaddle_);
    steps_.append(enableKey_);
    steps_.append(moveToCCPosition_);
    steps_.append(disableBrake_);
    steps_.append(switchCrystals_);
    steps_.append(enableBrake_);
    steps_.append(moveIntoNewRegion_);
    steps_.append(disableKey_);

    // Apply actions settings.

    setActions(crystalChangeActions);
}

BioXASSideMonoCrystalChangeSteps::~BioXASSideMonoCrystalChangeSteps()
{

}

int BioXASSideMonoCrystalChangeSteps::currentStepIndex() const
{
    return currentStepIndex_;
}

BioXASSideMonoCrystalChangeStep* BioXASSideMonoCrystalChangeSteps::currentStep() const
{
    return steps_.at(currentStepIndex_);
}

void BioXASSideMonoCrystalChangeSteps::setActions(AMAction3 *newActions)
{
    if (actions_ != newActions) {

        if (actions_)
            disconnect( actions_, 0, this, 0 );

        actions_ = qobject_cast<AMListAction3*>(newActions);

        if (actions_) {
            currentStepIndex_ = -1;
            connect( actions_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double, double)) );
            connect( actions_, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCurrentActionChanged(int)) );
            connect( actions_, SIGNAL(succeeded()), this, SIGNAL(stepsSucceeded()) );
            connect( actions_, SIGNAL(failed()), this, SIGNAL(stepsFailed()) );

            closeSlits_->setAction(actions_->subActionAt(0));
        }
    }
}

void BioXASSideMonoCrystalChangeSteps::setCurrentStepIndex(int newIndex)
{
    if (currentStepIndex_ != newIndex) {
        currentStepIndex_ = newIndex;
        emit currentStepChanged(steps_.at(currentStepIndex_));
    }
}

void BioXASSideMonoCrystalChangeSteps::start()
{
    if (actions_)
        actions_->start();

    emit stepsStarted();
}

void BioXASSideMonoCrystalChangeSteps::onCurrentActionChanged(int actionIndex)
{
    setCurrentStepIndex(actionIndex);
}









BioXASSideMonoCrystalChangeStep::BioXASSideMonoCrystalChangeStep(AMAction3 *action, const QString &description, const QString &userInstructions, QObject *parent) :
    QObject(parent)
{
    // Set variables.

    action_ = 0;
    description_ = description;
    userInstructions_ = userInstructions;
    status_ = "";

    // Apply action settings.

    setAction(action);
}

BioXASSideMonoCrystalChangeStep::~BioXASSideMonoCrystalChangeStep()
{

}

void BioXASSideMonoCrystalChangeStep::setAction(AMAction3 *newAction)
{
    if (action_ != newAction) {

        if (action_)
            disconnect( action_, 0, this, 0 );

        action_ = newAction;

        if (action_) {
            connect( action_, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()) );
        }
    }
}

void BioXASSideMonoCrystalChangeStep::setDescription(const QString &newDescription)
{
    if (description_ != newDescription) {
        description_ = newDescription;
        emit descriptionChanged(description_);
    }
}

void BioXASSideMonoCrystalChangeStep::setUserInstructions(const QString &newInstructions)
{
    if (userInstructions_ != newInstructions) {
        userInstructions_ = newInstructions;
        emit userInstructionsChanged(userInstructions_);
    }
}

void BioXASSideMonoCrystalChangeStep::onActionSucceeded()
{
    if (action_) {
        status_ = "DONE";
        setDescription(description_ + status_);
    }
}
