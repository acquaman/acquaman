/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMActionRunnerAddActionBar3.h"

#include <QSpinBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QBoxLayout>
#include <QStringBuilder>
#include <QMessageBox>

#include "actions3/AMActionRunner3.h"
#include "util/AMDateTimeUtils.h"

 AMActionRunnerAddActionBar3::~AMActionRunnerAddActionBar3(){}
AMActionRunnerAddActionBar3::AMActionRunnerAddActionBar3(const QString& actionCategoryName, bool enableRepeatScans, QWidget *parent) :
	QWidget(parent)
{
	actionCategoryName_ = actionCategoryName;
	repeatScansEnabled_ = enableRepeatScans;
	estimatedTimeOfSingleScan_ = 0;

	overallScanTime_ = new QLabel("Estimated scan time: 0s");

	iterationsBox_ = new QSpinBox;
	iterationsBox_->setRange(1, 1000000);
	iterationsBox_->setPrefix("#: ");
	iterationsBox_->setAlignment(Qt::AlignCenter);
	iterationsBox_->setValue(1);

	startActionButton_ = new QPushButton("Start " % actionCategoryName_);
	addToQueueButton_ = new QPushButton("Add to Workflow");

	whenDoneLabel_ = new QLabel("When done here:");
	goToWorkflowOption_ = new QRadioButton("Show the workflow");
	setupAnotherActionOption_ = new QRadioButton("Setup another " % actionCategoryName_);

	QButtonGroup* bg = new QButtonGroup(this);
	bg->addButton(goToWorkflowOption_, 0);
	bg->addButton(setupAnotherActionOption_, 1);
	goToWorkflowOption_->setChecked(true);

	layout_ = new QVBoxLayout();

	QHBoxLayout* optionsHL = new QHBoxLayout();
	optionsHL->addWidget(whenDoneLabel_);
	optionsHL->addWidget(goToWorkflowOption_);
	optionsHL->addWidget(setupAnotherActionOption_);
	optionsHL->addStretch();

	if (repeatScansEnabled_) {
		optionsHL->addWidget(overallScanTime_);
		optionsHL->addWidget(new QLabel("<b>Iterations:</b>"));
		optionsHL->addWidget(iterationsBox_);
	}
	optionsHL->addWidget(addToQueueButton_);
	optionsHL->addWidget(startActionButton_);
	optionsHL->setContentsMargins(10, 0, 10, 20);

	layout_->addLayout(optionsHL);
	layout_->setContentsMargins(0,0,0,0);

	setLayout(layout_);

	connect(startActionButton_, SIGNAL(clicked()), this, SLOT(onStartActionRequested()));
	connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));
	connect(iterationsBox_, SIGNAL(valueChanged(int)), this, SLOT(onScanIterationsValueChange()));

	connect(AMActionRunner3::workflow(), SIGNAL(currentActionChanged(AMAction3*)), this, SLOT(reviewStartActionButtonState()));

	reviewStartActionButtonState();
}

void AMActionRunnerAddActionBar3::onScanIterationsValueChange()
{
	double totalTime = estimatedTimeOfSingleScan_ * iterationsBox_->value();
	updateOverallScanTimeLabel(totalTime);
}

void AMActionRunnerAddActionBar3::updateOverallScanTime(double seconds)
{
	estimatedTimeOfSingleScan_ = seconds;
	double totalTime = estimatedTimeOfSingleScan_ * iterationsBox_->value();
	updateOverallScanTimeLabel(totalTime);
}

void AMActionRunnerAddActionBar3::onStartActionRequested(){

	if(AMActionRunner3::workflow()->actionRunning())
		return;

	// check first: if there's already items queue up in the workflow, we need to find out if they want to add this action to the end of the queue and run, add this action to the beginning of the queue and run, add to end but not run, or just run this one first and leave the rest of the queue alone.
	QAbstractButton* result = 0;
	QPushButton* cancel=0, *addToBeginningAndStart=0, *addToEndAndStart=0, *addToEnd=0, *runOnlyThisOne=0;
	if(AMActionRunner3::workflow()->queuedActionCount()) {

		QMessageBox questionBox;
		questionBox.setText("There are already actions waiting in the workflow queue.");
		questionBox.setInformativeText("What do you want to do with this " % actionCategoryName_ % "?");
		questionBox.setIcon(QMessageBox::Question);

		cancel = questionBox.addButton("Cancel", QMessageBox::RejectRole);
		addToBeginningAndStart = questionBox.addButton("Add to beginning and start workflow", QMessageBox::YesRole);
		addToEndAndStart = questionBox.addButton("Add to end and start workflow", QMessageBox::YesRole);
		addToEnd = questionBox.addButton("Add to end", QMessageBox::YesRole);
		runOnlyThisOne = questionBox.addButton("Run just this " % actionCategoryName_, QMessageBox::YesRole);

		questionBox.setDefaultButton(runOnlyThisOne);
		questionBox.exec();

		result = questionBox.clickedButton();
		if(result == cancel) {
			return;
		}
	}

	ActionQueue::QueueOperation operation = ActionQueue::AddToEnd;
	if (result == 0) { // no other actions in queue... Just run now.
		operation = ActionQueue::RunAtOnce;

	} else if (result == runOnlyThisOne) { // there are other actions, but we only want to run this one.
		operation = ActionQueue::RunOnlyThisOne;

	} else if (result == addToBeginningAndStart) {
		operation = ActionQueue::AddToBeginningAndStart;

	} else if (result == addToEndAndStart) {
		operation = ActionQueue::AddToEndAndStart;

	} else if (result == addToEnd) {
		operation = ActionQueue::AddToEnd;
	}

	addActionToQueue(operation);
}


void AMActionRunnerAddActionBar3::onAddToQueueRequested() {
	addActionToQueue(ActionQueue::AddToEnd);
}

void AMActionRunnerAddActionBar3::reviewStartActionButtonState() {

	// if we can't start the action immediately because another action is already running in the workflow.
	if(AMActionRunner3::workflow()->actionRunning()) {
		startActionButton_->setEnabled(false);
		addToQueueButton_->setEnabled(true);
		startActionButton_->setText("[Actions currently running]");
	}
	// Good to go. The workflow isn't running an action right now, but watch out: there may or may not still be  actions already in the queue. We'll need to add this action to the front of the queue, and then start it.
	else {
		startActionButton_->setEnabled(true);
		addToQueueButton_->setEnabled(true);
		startActionButton_->setText("Start " % actionCategoryName_);
	}
}


void AMActionRunnerAddActionBar3::addWidget(QWidget *widget)
{
	if(!widget)
		return;

	layout_->insertWidget(0, widget);

}

void AMActionRunnerAddActionBar3::addActionToQueue(ActionQueue::QueueOperation operation)
{
	AMAction3 * action = repeatScansEnabled_ ? createMultipleScans() : createScan();
	if (!action)
		return;

	AMActionRunner3* workflow = AMActionRunner3::workflow();

	if (operation == ActionQueue::RunOnlyThisOne) {
		workflow->runActionImmediatelyInQueue(action);

	} else {
		// add actions to queue
		int insertIndex = ActionQueue::insertTaskToBeginning(operation) ? 0: -1;
		workflow->insertActionInQueue(action, insertIndex);

		// start the queue
		if (ActionQueue::startQueue(operation)) {
			workflow->setQueuePaused(false);
		}
	}

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}

void AMActionRunnerAddActionBar3::updateOverallScanTimeLabel(double seconds)
{
	overallScanTime_->setText(QString("<b>Estimated overall time:</b> %1").arg(AMDateTimeUtils::convertTimeToString(seconds)));
}
