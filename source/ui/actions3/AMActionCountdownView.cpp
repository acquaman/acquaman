#include "AMActionCountdownView.h"

AMActionCountdownView::AMActionCountdownView(AMAction3 *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	countdownLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(countdownLabel_);

	setLayout(layout);

	// Current settings.

	setAction(action);

	refresh();
}

AMActionCountdownView::~AMActionCountdownView()
{

}

void AMActionCountdownView::clear()
{
	countdownLabel_->clear();
}

void AMActionCountdownView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateCountdownLabel();
}

void AMActionCountdownView::setAction(AMAction3 *newAction)
{
	if (action_ != newAction) {

		if (action_) {
			disconnect( action_, 0, this, 0 );

			setValue(0);
		}

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(stateChanged(int,int)), this, SLOT(onActionStateChanged(int, int)) );
			connect( action_, SIGNAL(progressChanged(double,double)), this, SLOT(onActionProgressChanged(double, double)) );

			setValue( progressRemaining(action_->progress().first, action_->progress().second) );
		}

		emit actionChanged(action_);
	}
}

void AMActionCountdownView::setValue(int newCount)
{
	if (value_ != newCount) {
		value_ = newCount;

		refresh();
	}
}

void AMActionCountdownView::updateCountdownLabel()
{
	countdownLabel_->setText(QString::number(value_, 'f', 0));
}

void AMActionCountdownView::onActionStateChanged(int oldState, int newState)
{
	Q_UNUSED(oldState)

	if ((newState == AMAction3::Cancelled) || (newState == AMAction3::Succeeded) || (newState == AMAction3::Failed) || (newState == AMAction3::Skipping))
		action_ = 0;
}

void AMActionCountdownView::onActionProgressChanged(double currentValue, double totalValue)
{
	setValue(progressRemaining(currentValue, totalValue));
}

double AMActionCountdownView::progressRemaining(double currentProgress, double totalProgress)
{
	return totalProgress - currentProgress;
}

