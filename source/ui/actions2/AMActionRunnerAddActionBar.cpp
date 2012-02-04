#include "AMActionRunnerAddActionBar.h"

#include "actions2/AMActionRunner.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QBoxLayout>

#include <QStringBuilder>

AMActionRunnerAddActionBar::AMActionRunnerAddActionBar(const QString& actionCategoryName, QWidget *parent) :
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

	connect(AMActionRunner::s(), SIGNAL(currentActionChanged(AMAction*)), this, SLOT(reviewStartActionButtonState()));

	reviewStartActionButtonState();
}


#include <QMessageBox>
void AMActionRunnerAddActionBar::onStartActionRequested(){

	if(AMActionRunner::s()->actionRunning())
		return;

	// check first: if there's already items queue up in the workflow, we need to find out if they want to add this action to the end of the queue and run, add this action to the beginning of the queue and run, add to end but not run, or just run this one first and leave the rest of the queue alone.
	QAbstractButton* result = 0;
	QPushButton* cancel=0, *addToBeginningAndStart=0, *addToEndAndStart=0, *addToEnd=0, *runOnlyThisOne=0;
	if(AMActionRunner::s()->queuedActionCount()) {

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

	AMAction* action = createAction();
	if(!action)
		return;

	if(result == 0) {
		// no other actions in queue... Just run now.
		bool queuePaused = AMActionRunner::s()->queuePaused();
		AMActionRunner::s()->insertActionInQueue(action, 0);
		AMActionRunner::s()->setQueuePaused(false);	// this will start it running.
		AMActionRunner::s()->setQueuePaused(queuePaused);	// leave the queue state how we found it.
	}

	else if(result == addToEnd) {
		AMActionRunner::s()->addActionToQueue(action);
	}
	else if(result == addToEndAndStart) {
		AMActionRunner::s()->addActionToQueue(action);
		AMActionRunner::s()->setQueuePaused(false);
	}
	else if(result == addToBeginningAndStart) {
		AMActionRunner::s()->insertActionInQueue(action, 0);
		AMActionRunner::s()->setQueuePaused(false);
	}
	else if(result == runOnlyThisOne) {
		// there are other actions, but we only want to run this one. Insert it at the front of the queue, un-pause the queue to start it up, and then pause it again.
		AMActionRunner::s()->insertActionInQueue(action, 0);
		AMActionRunner::s()->setQueuePaused(false);
		AMActionRunner::s()->setQueuePaused(true);
	}
}


void AMActionRunnerAddActionBar::onAddToQueueRequested() {

	AMAction* action = createAction();
	if(!action)
		return;

	AMActionRunner::s()->addActionToQueue(action);

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}


void AMActionRunnerAddActionBar::reviewStartActionButtonState() {

	// if we can't start the action immediately because another action is already running in the workflow.
	if(AMActionRunner::s()->actionRunning()) {
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


void AMActionRunnerAddActionBar::addWidget(QWidget *widget)
{
	if(!widget)
		return;

	layout_->insertWidget(0, widget);
}
