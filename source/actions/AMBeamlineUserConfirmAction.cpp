/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMBeamlineUserConfirmAction.h"

#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

AMBeamlineUserConfirmAction::AMBeamlineUserConfirmAction(QObject *parent) :
		AMBeamlineActionItem(parent)
{
	setReady(true);
}

AMBeamlineActionItemView* AMBeamlineUserConfirmAction::createView(int index){
	Q_UNUSED(index)
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

	helpButton_ = new QToolButton();
	helpButton_->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	connect(helpButton_, SIGNAL(clicked()), this, SLOT(onHelpButtonClicked()));

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
		if(userConfirmAction_->hasHelp())
			mainHL_->addWidget(helpButton_);
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
	Q_UNUSED(explanation)
}

void AMBeamlineUserConfirmDetailedActionView::onHelpButtonClicked(){
	AMImageListView *helpImagesView = new AMImageListView(userConfirmAction_->helpImages());
	helpImagesView->show();
}
