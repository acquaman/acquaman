#include "AMActionRunnerAddActionBar3.h"

#include "actions3/AMActionRunner3.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QBoxLayout>

#include <QStringBuilder>

AMActionRunnerAddActionBar3::AMActionRunnerAddActionBar3(const QString& actionCategoryName, QWidget *parent) :
	QWidget(parent)
{
	actionCategoryName_ = actionCategoryName;

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
	optionsHL->addWidget(addToQueueButton_);
	optionsHL->addWidget(startActionButton_);
	optionsHL->setContentsMargins(10, 0, 10, 20);

	layout_->addLayout(optionsHL);
	layout_->setContentsMargins(0,0,0,0);

	setLayout(layout_);

	connect(startActionButton_, SIGNAL(clicked()), this, SLOT(onStartActionRequested()));
	connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));

	connect(AMActionRunner3::s(), SIGNAL(currentActionChanged(AMAction3*)), this, SLOT(reviewStartActionButtonState()));

	reviewStartActionButtonState();
}


#include <QMessageBox>
void AMActionRunnerAddActionBar3::onStartActionRequested(){

	if(AMActionRunner3::s()->actionRunning())
		return;

	// check first: if there's already items queue up in the workflow, we need to find out if they want to add this action to the end of the queue and run, add this action to the beginning of the queue and run, add to end but not run, or just run this one first and leave the rest of the queue alone.
	QAbstractButton* result = 0;
	QPushButton* cancel=0, *addToBeginningAndStart=0, *addToEndAndStart=0, *addToEnd=0, *runOnlyThisOne=0;
	if(AMActionRunner3::s()->queuedActionCount()) {

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

	AMAction3* action = createAction();
	if(!action)
		return;

	if(result == 0) {
		// no other actions in queue... Just run now.
		AMActionRunner3::s()->runActionImmediatelyInQueue(action);
	}

	else if(result == addToEnd) {
		AMActionRunner3::s()->addActionToQueue(action);
	}
	else if(result == addToEndAndStart) {
		AMActionRunner3::s()->addActionToQueue(action);
		AMActionRunner3::s()->setQueuePaused(false);
	}
	else if(result == addToBeginningAndStart) {
		AMActionRunner3::s()->insertActionInQueue(action, 0);
		AMActionRunner3::s()->setQueuePaused(false);
	}
	else if(result == runOnlyThisOne) {
		// there are other actions, but we only want to run this one.
		AMActionRunner3::s()->runActionImmediatelyInQueue(action);
	}
}


void AMActionRunnerAddActionBar3::onAddToQueueRequested() {

	AMAction3* action = createAction();
	if(!action)
		return;

	AMActionRunner3::s()->addActionToQueue(action);

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}


void AMActionRunnerAddActionBar3::reviewStartActionButtonState() {

	// if we can't start the action immediately because another action is already running in the workflow.
	if(AMActionRunner3::s()->actionRunning()) {
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
