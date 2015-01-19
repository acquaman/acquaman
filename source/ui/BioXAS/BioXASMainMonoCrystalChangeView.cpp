#include "BioXASMainMonoCrystalChangeView.h"
#include <QDebug>

BioXASMainMonoCrystalChangeView::BioXASMainMonoCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // set variables.

    mono_ = 0;

    promptMessage_ = "Begin crystal change procedure?";
    successMessage_ = "Crystal change completed successfully!";
    failMessage_ = "Unable to complete crystal change procedure.";

    // create UI elements.

    startView_ = new BioXASMainMonoCrystalChangeViewMessage(this);
    actionsView_ = new BioXASMainMonoCrystalChangeViewActions(this);
    finalView_ = new BioXASMainMonoCrystalChangeViewMessage(this);

    // create and set layouts.

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(startView_);
    layout->addWidget(actionsView_);
    layout->addWidget(finalView_);

    setLayout(layout);

    // make connections.

    connect( startView_, SIGNAL(okClicked()), this, SLOT(showActionsView()) );
    connect( startView_, SIGNAL(closeClicked()), this, SLOT(close()) );
    connect( actionsView_, SIGNAL(actionsComplete(bool)), this, SLOT(onActionsComplete(bool)) );
    connect( finalView_, SIGNAL(closeClicked()), this, SLOT(close()) );

    // initial settings.

    startView_->hide();
    actionsView_->hide();
    finalView_->hide();

    startView_->setMessage(promptMessage_);
    startView_->setOKButtonShown(true);
    startView_->setOKButtonEnabled(true);
    startView_->setOKButtonDefault(true);
    startView_->setCloseButtonShown(true);
    startView_->setCloseButtonEnabled(true);

//    finalView_->setMessage(successMessage_);
    finalView_->setOKButtonShown(false);
    finalView_->setCloseButtonShown(true);
    finalView_->setCloseButtonDefault(true);

    // current settings.

    showStartView();

    setMono(mono);
}

BioXASMainMonoCrystalChangeView::~BioXASMainMonoCrystalChangeView()
{

}

void BioXASMainMonoCrystalChangeView::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_) {
            disconnect( mono_, 0, this, 0 );
            actionsView_->setActions(0);
        }

        mono_ = newMono;

        if (mono_) {

            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged(bool)) );
            actionsView_->setActions(mono_->createCrystalChangeAction());
            onMonoConnectedChanged(mono_->isConnected());

        } else {
            onMonoConnectedChanged(false);
        }


        emit monoChanged(mono_);
    }
}

void BioXASMainMonoCrystalChangeView::showStartView()
{
    // hiding the actions view cancels the crystal change actions.

    if (actionsView_->isVisible())
        cancelCrystalChange();

    startView_->show();
    actionsView_->hide();
    finalView_->hide();
}

void BioXASMainMonoCrystalChangeView::showActionsView()
{
    startView_->hide();
    actionsView_->show();
    finalView_->hide();

    // showing the actions view starts the crystal change actions.

    startCrystalChange();
}

void BioXASMainMonoCrystalChangeView::showFinalView()
{
    // hiding the actions view cancels the crystal change actions.

    if (actionsView_->isVisible())
        cancelCrystalChange();

    startView_->hide();
    actionsView_->hide();
    finalView_->show();
}

void BioXASMainMonoCrystalChangeView::onMonoConnectedChanged(bool isConnected)
{
    if (!isConnected) {
        finalView_->setMessage(failMessage_);
        showFinalView();

    } else if (isConnected) {
        actionsView_->setActions(mono_->createCrystalChangeAction());
        showStartView();
    }
}

void BioXASMainMonoCrystalChangeView::onActionsComplete(bool successful)
{
    if (successful)
        finalView_->setMessage(successMessage_);

    else
        finalView_->setMessage(failMessage_);

    showFinalView();
}

bool BioXASMainMonoCrystalChangeView::startCrystalChange()
{
    bool result = false;

    if (actionsView_->actions())
        result = actionsView_->actions()->start();
    else
        qDebug() << "No crystal change actions provided.";

    return result;
}

bool BioXASMainMonoCrystalChangeView::cancelCrystalChange()
{
    bool result = false;

    if (actionsView_->actions())
        result = actionsView_->actions()->cancel();

    return result;
}

//

BioXASMainMonoCrystalChangeViewMessage::BioXASMainMonoCrystalChangeViewMessage(QWidget *parent, const QString &message, bool showOKButton, bool showCloseButton) :
    QWidget(parent)
{
    // create UI elements.

    message_ = new QLabel(this);
    okButton_ = new QPushButton("Ok", this);
    closeButton_ = new QPushButton("Close", this);

    // create and set layouts.

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(closeButton_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(message_);
    layout->addLayout(buttonLayout);

    setLayout(layout);

    // make connections.

    connect( okButton_, SIGNAL(clicked()), this, SIGNAL(okClicked()) );
    connect( closeButton_, SIGNAL(clicked()), this, SIGNAL(closeClicked()) );

    // current settings.

    setMessage(message);
    setOKButtonShown(showOKButton);
    setCloseButtonShown(showCloseButton);
}

BioXASMainMonoCrystalChangeViewMessage::~BioXASMainMonoCrystalChangeViewMessage()
{

}

//

BioXASMainMonoCrystalChangeViewActions::BioXASMainMonoCrystalChangeViewActions(QWidget *parent, AMAction3 *actions) :
    QWidget(parent)
{
    // set variables.

    actions_ = 0;

    // create UI elements.

    progressBar_ = new QProgressBar(this);
    actionView_ = new BioXASMainMonoCrystalChangeViewAction(0, this);

    // create and set layouts.

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progressBar_);
    layout->addWidget(actionView_);

    setLayout(layout);

    // initial settings.

    progressBar_->setMinimum(0);
    progressBar_->setMaximum(10);
    progressBar_->setValue(0);

    // current settings.

    setActions(actions);
}

BioXASMainMonoCrystalChangeViewActions::~BioXASMainMonoCrystalChangeViewActions()
{

}

void BioXASMainMonoCrystalChangeViewActions::setActions(AMAction3 *newActions)
{
    if (actions_ != newActions) {

        // disconnect from the current list of actions, clear current action view.

        if (actions_) {
            disconnect( actions_, 0, this, 0 );
        }

        actionView_->setAction(0);

        // accept new actions as the current list of actions.

        actions_ = qobject_cast<AMListAction3*>(newActions);

        // connect to new actions. current action view should update once actions begin execution.

        if (actions_) {
            connect( actions_, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCurrentSubActionChanged(int)) );
            connect( actions_, SIGNAL(succeeded()), this, SLOT(onActionsSucceeded()) );
            connect( actions_, SIGNAL(cancelled()), this, SLOT(onActionsCancelled()) );
            connect( actions_, SIGNAL(failed()), this, SLOT(onActionsFailed()) );
            connect( actions_, SIGNAL(progressChanged(double,double)), this, SLOT(onActionsProgressChanged(double, double)) );
        }
    }
}

void BioXASMainMonoCrystalChangeViewActions::onCurrentSubActionChanged(int newIndex)
{
    if (actions_) {

        // update the view to display the info for the new current action.
        actionView_->setAction(actions_->subActionAt(newIndex));
    }
}

void BioXASMainMonoCrystalChangeViewActions::onActionsProgressChanged(double num, double denom)
{
    progressBar_->setMaximum(denom);
    progressBar_->setValue(num);
}

//

BioXASMainMonoCrystalChangeViewAction::BioXASMainMonoCrystalChangeViewAction(AMAction3 *action, QWidget *parent) :
    QWidget(parent)
{
    // set variables.

    action_ = 0;

    // create UI elements.

    description_ = new QLabel(this);
    instructions_ = new QLabel(this);

    // create and set layouts.

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(description_);
    layout->addWidget(instructions_);

    setLayout(layout);

    // current settings.

    setAction(action);
}

BioXASMainMonoCrystalChangeViewAction::~BioXASMainMonoCrystalChangeViewAction()
{

}

void BioXASMainMonoCrystalChangeViewAction::clear()
{
    description_->clear();
    instructions_->clear();
}

void BioXASMainMonoCrystalChangeViewAction::update()
{
    if (action_) {
        description_->setText(action_->info()->shortDescription());
        instructions_->setText(action_->info()->longDescription());

    } else {
        description_->setText("");
        instructions_->setText("");
    }
}

void BioXASMainMonoCrystalChangeViewAction::setAction(AMAction3 *newAction)
{
    if (action_ != newAction) {

        action_ = newAction;

        // update the view to reflect new action information.
        update();

        emit actionChanged(action_);
    }
}
