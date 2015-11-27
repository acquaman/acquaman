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
		}

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(progressChanged(double,double)), this, SLOT(updateCountdownLabel()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void AMActionCountdownView::updateCountdownLabel()
{
	if (action_) {
		QPair<double, double> progress = action_->progress();
		double countdown = progress.second - progress.first;
		countdownLabel_->setText(QString::number(countdown, 'f', 0));

	} else {
		countdownLabel_->setText("");
	}
}

