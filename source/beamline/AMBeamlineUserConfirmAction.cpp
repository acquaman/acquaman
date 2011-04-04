#include "AMBeamlineUserConfirmAction.h"

AMBeamlineUserConfirmAction::AMBeamlineUserConfirmAction(QObject *parent) :
		AMBeamlineActionItem(parent)
{
	setReady(true);
}

AMBeamlineActionItemView* AMBeamlineUserConfirmAction::createView(int index){
	return new AMBeamlineUserConfirmDetailedActionView(this);
}

void AMBeamlineUserConfirmAction::start(){
	setStarted(true);
}

void AMBeamlineUserConfirmAction::cancel(){
	setFailed(true);
}

void AMBeamlineUserConfirmAction::cleanup(){

}

void AMBeamlineUserConfirmAction::userProceed(){
	setSucceeded(true);
}

void AMBeamlineUserConfirmAction::userCancel(){
	cancel();
}


AMBeamlineUserConfirmDetailedActionView::AMBeamlineUserConfirmDetailedActionView(AMBeamlineUserConfirmAction *userConfirmAction, int index, QWidget *parent) :
		AMBeamlineActionItemView(userConfirmAction, index, parent)
{
	messageLabel_ = 0; //NULL
	userConfirmAction_ = 0; //NULL
	setAction(userConfirmAction);

	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	messageLabel_ = new QLabel();
	messageLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	proceedButton_ = new QPushButton("Proceed");
	proceedButton_->setEnabled(userConfirmAction_ && userConfirmAction_->isRunning());
	connect(proceedButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	cancelButton_ = new QPushButton("Cancel");
	cancelButton_->setEnabled(userConfirmAction_ && userConfirmAction_->isRunning());
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));

	buttonsVL_ = new QVBoxLayout();
	buttonsVL_->addWidget(proceedButton_);
	buttonsVL_->setSpacing(0);
	buttonsVL_->addWidget(cancelButton_);

	finishedState_ = new QToolButton();
	finishedState_->setContentsMargins(0, 0, 0, 0);
	finishedState_->setEnabled(false);
	finishedState_->setIcon(QIcon(":/greenCheck.png"));

	mainHL_ = new QHBoxLayout();
	mainHL_->addWidget(messageLabel_);
	mainHL_->addLayout(buttonsVL_);

	setLayout(mainHL_);
	QMargins mainHLMargins = mainHL_->contentsMargins();
	mainHLMargins.setTop(1);
	mainHLMargins.setBottom(1);
	mainHL_->setContentsMargins(mainHLMargins);

	onInfoChanged();
}

void AMBeamlineUserConfirmDetailedActionView::setIndex(int index){
	index_ = index;
	onInfoChanged();
}

void AMBeamlineUserConfirmDetailedActionView::setAction(AMBeamlineActionItem *action){
	AMBeamlineUserConfirmAction *userConfirmAction = qobject_cast<AMBeamlineUserConfirmAction*>(action);
	if(userConfirmAction_){
		disconnect(userConfirmAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
		disconnect(userConfirmAction_, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		disconnect(userConfirmAction_, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
	}
	userConfirmAction_ = userConfirmAction;
	if(userConfirmAction_){
		connect(userConfirmAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
		connect(userConfirmAction_, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		connect(userConfirmAction_, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
	}
	onInfoChanged();
}

void AMBeamlineUserConfirmDetailedActionView::onInfoChanged(){
	if(userConfirmAction_ && messageLabel_){
		messageLabel_->setText(userConfirmAction_->message());
	}
}

void AMBeamlineUserConfirmDetailedActionView::onPlayPauseButtonClicked(){
	userConfirmAction_->userProceed();
}

void AMBeamlineUserConfirmDetailedActionView::onStopCancelButtonClicked(){
	userConfirmAction_->userCancel();
}

void AMBeamlineUserConfirmDetailedActionView::onActionStarted(){
	proceedButton_->setEnabled(true);
	cancelButton_->setEnabled(true);
	emit actionStarted(userConfirmAction_);
}

void AMBeamlineUserConfirmDetailedActionView::onActionSucceeded(){
	mainHL_->removeItem(buttonsVL_);
	proceedButton_->hide();
	cancelButton_->hide();
	mainHL_->addWidget(finishedState_);
	emit actionSucceeded(userConfirmAction_);
}

void AMBeamlineUserConfirmDetailedActionView::onActionFailed(int explanation){

}
