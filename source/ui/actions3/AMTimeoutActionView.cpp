#include "AMTimeoutActionView.h"

AMTimeoutActionView::AMTimeoutActionView(AMTimeoutAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	instructionLabel_ = new QLabel();

	remainingTimeLabel_ = new QLabel();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(instructionLabel_);
	layout->addWidget(remainingTimeLabel_);

	setLayout(layout);

	// Current settings.

	setAction(action);
	refresh();
}

AMTimeoutActionView::~AMTimeoutActionView()
{

}

void AMTimeoutActionView::clear()
{
	instructionLabel_->clear();
	remainingTimeLabel_->clear();
}

void AMTimeoutActionView::refresh()
{
	// Clear the view.

	clear();

	// Update the view elements.

	updateInstructionLabel();
	updateRemainingTimeLabel();
}

void AMTimeoutActionView::setAction(AMTimeoutAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_) {
			connect( action_->timeoutActionInfo(), SIGNAL(instructionChanged(QString)), this, SLOT(updateInstructionLabel()) );
			connect( action_, SIGNAL(progressChanged(double,double)), this, SLOT(updateRemainingTimeLabel()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void AMTimeoutActionView::setActionSucceeded()
{
	if (action_)
		action_->proceed();
}

void AMTimeoutActionView::updateInstructionLabel()
{
	if (action_) {

		// Find the latest information on the action's instruction.

		QString instruction;

		if (action_->timeoutActionInfo())
			instruction = action_->timeoutActionInfo()->instruction();

		// Update the instruction label.

		instructionLabel_->setText(instruction);

	} else {

		// If there is no action information available, clear the label.

		instructionLabel_->clear();
	}
}

void AMTimeoutActionView::updateRemainingTimeLabel()
{
	if (action_) {

		// Find the latest information on the action's wait time.

		QPair<double, double> currentProgress = action_->progress();

		// Identify the remaining time. The action reports the time
		// in seconds.

		double remainingTime = currentProgress.second - currentProgress.first;

		// Update the remaining time label.

		remainingTimeLabel_->setText(QString::number(remainingTime) + " s");

	} else {

		// If there is no action information available, clear the label.

		remainingTimeLabel_->clear();
	}
}


