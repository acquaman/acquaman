#include "AMActionStatusView.h"

AMActionStatusView::AMActionStatusView(AMAction3 *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	statusLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(statusLabel_);

	setLayout(layout);

	// Current settings.

	setAction(action);

	refresh();
}

AMActionStatusView::~AMActionStatusView()
{

}

void AMActionStatusView::clear()
{
	statusLabel_->clear();
}

void AMActionStatusView::refresh()
{
	// Clears the view.

	clear();

	// Update view elements.

	updateStatusLabel();
}

void AMActionStatusView::setAction(AMAction3 *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_)
			connect( action_, SIGNAL(statusTextChanged(QString)), this, SLOT(updateStatusLabel()) );

		refresh();

		emit actionChanged(action_);
	}
}

void AMActionStatusView::updateStatusLabel()
{
	QString statusText = "No action set.";

	if (action_)
		statusText = action_->statusText();

	statusLabel_->setText(statusText);
}

