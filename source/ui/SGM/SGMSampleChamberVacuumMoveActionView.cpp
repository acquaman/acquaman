#include "SGMSampleChamberVacuumMoveActionView.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"
#include "actions3/actions/AMTimeoutAction.h"

SGMSampleChamberVacuumMoveActionStatusView::SGMSampleChamberVacuumMoveActionStatusView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
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

SGMSampleChamberVacuumMoveActionStatusView::~SGMSampleChamberVacuumMoveActionStatusView()
{

}

void SGMSampleChamberVacuumMoveActionStatusView::clear()
{
	statusLabel_->clear();
}

void SGMSampleChamberVacuumMoveActionStatusView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateStatusLabel();
}

void SGMSampleChamberVacuumMoveActionStatusView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_)
			connect( action_, SIGNAL(stateChanged(int,int)), this, SLOT(updateStatusLabel()) );

		refresh();

		emit actionChanged(action_);
	}
}

void SGMSampleChamberVacuumMoveActionStatusView::updateStatusLabel()
{
	QString newText = "No action set.";

	if (action_)
		newText = stateToString(action_->state());

	statusLabel_->setText(newText);
}

QString SGMSampleChamberVacuumMoveActionStatusView::stateToString(AMAction3::State state) const
{
	switch (state) {
	case AMAction3::Constructed:
		return "Sample chamber vacuum move action hasn't started yet.";
	case AMAction3::Starting:
		return "Sample chamber vacuum move action is starting...";
	case AMAction3::Running:
		return "Sample chamber vacuum move action is running.";
	case AMAction3::Pausing:
		return "Sample chamber vacuum move action is pausing...";
	case AMAction3::Paused:
		return "Sample chamber vacuum move action has paused.";
	case AMAction3::Resuming:
		return "Sample chamber vacuum move action is resuming...";
	case AMAction3::Cancelling:
		return "Sample chamber vacuum move action is cancelling...";
	case AMAction3::Cancelled:
		return "Sample chamber vacuum move action has cancelled.";
	case AMAction3::Succeeded:
		return "Sample chamber vacuum move action has succeeded!";
	case AMAction3::Failed:
		return "Sample chamber vacuum move action has FAILED.";
	case AMAction3::Skipping:
		return "Skipping the sample chamber vacuum move action.";
	default:
		return "Sample chamber vacuum move action has encountered an invalid state (!!).";
	}
}

SGMSampleChamberVacuumMoveActionView::SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	descriptionView_ = new AMActionInfoDescriptionView(0);

	statusView_ = new SGMSampleChamberVacuumMoveActionStatusView(0);

	countdownView_ = new AMActionCountdownView(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(descriptionView_);
	layout->addWidget(statusView_);
	layout->addWidget(countdownView_);

	setLayout(layout);

	// Current settings.

	setAction(action);

	refresh();
}

SGMSampleChamberVacuumMoveActionView::~SGMSampleChamberVacuumMoveActionView()
{

}

void SGMSampleChamberVacuumMoveActionView::clear()
{
	descriptionView_->clear();
	statusView_->clear();
	countdownView_->clear();
}

void SGMSampleChamberVacuumMoveActionView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateDescriptionView();
	updateStatusView();
	updateCountdownView();
}

void SGMSampleChamberVacuumMoveActionView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateDescriptionView()) );
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateCountdownView()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void SGMSampleChamberVacuumMoveActionView::updateDescriptionView()
{
	// Update the action being viewed.

	if (action_ && action_->currentSubAction())
		descriptionView_->setInfo(action_->currentSubAction()->info());
	else
		descriptionView_->setInfo(0);

	// Update the view's visibility.

	bool showView = false;

	if (action_ && action_->inFinalState())
		showView = false;
	else
		showView = true;

	if (showView)
		descriptionView_->show();
	else
		descriptionView_->hide();
}

void SGMSampleChamberVacuumMoveActionView::updateStatusView()
{
	// Update the action being viewed.

	statusView_->setAction(action_);

	// Update the view's visibility.

	bool showView = false;

	if (action_ && action_->inFinalState())
		showView = true;

	if (showView)
		statusView_->show();
	else
		statusView_->hide();
}

void SGMSampleChamberVacuumMoveActionView::updateCountdownView()
{
	if (action_ && action_->currentSubAction())
		countdownView_->setAction(action_->currentSubAction());
	else
		countdownView_->setAction(0);
}

SGMSampleChamberVacuumMoveActionDialog::SGMSampleChamberVacuumMoveActionDialog(SGMSampleChamberVacuumMoveAction *action, QWidget *parent):
	QDialog(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	moveView_ = new SGMSampleChamberVacuumMoveActionView(0);

	okButton_ = new QPushButton("Ok");

	cancelButton_ = new QPushButton("Cancel");

	buttonBox_ = new QDialogButtonBox();
	buttonBox_->addButton(okButton_, QDialogButtonBox::AcceptRole);
	buttonBox_->addButton(cancelButton_, QDialogButtonBox::RejectRole);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(moveView_);
	layout->addWidget(buttonBox_);

	setLayout(layout);

	// Make connections.

	connect( buttonBox_, SIGNAL(accepted()), this, SLOT(onOKButtonClicked()) );
	connect( buttonBox_, SIGNAL(rejected()), this, SLOT(onCancelButtonClicked()) );

	// Current settings.

	setAction(action);

	refresh();
}

SGMSampleChamberVacuumMoveActionDialog::~SGMSampleChamberVacuumMoveActionDialog()
{

}

void SGMSampleChamberVacuumMoveActionDialog::clear()
{
	okButton_->hide();
	moveView_->setAction(0);
}

void SGMSampleChamberVacuumMoveActionDialog::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateMoveActionView();
	updateButtons();
}

void SGMSampleChamberVacuumMoveActionDialog::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateButtons()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void SGMSampleChamberVacuumMoveActionDialog::updateMoveActionView()
{
	moveView_->setAction(action_);
}

void SGMSampleChamberVacuumMoveActionDialog::updateButtons()
{
	updateOKButton();
	updateCancelButton();
}

void SGMSampleChamberVacuumMoveActionDialog::updateOKButton()
{
	// We expect to see an OK button when the move action needs user input
	// to proceed (ie if the currently executing action is a timeout action)
	// or when the action has reached a final state.

	bool showButton = false;

	if (action_) {

		if (action_->inFinalState()) {
			showButton = true;

		} else if (action_->state() == AMAction3::Running) {
			AMAction3 *currentAction = action_->currentSubAction();

			if (currentAction) {
				AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(currentAction);

				if (timeoutAction)
					showButton = true;
			}
		}
	}

	if (showButton)
		okButton_->show();
	else
		okButton_->hide();
}

void SGMSampleChamberVacuumMoveActionDialog::updateCancelButton()
{
	// We expect the Cancel button to be visible pretty much always, except when
	// the action has reached a final state (and can't be cancelled).

	if (action_ && action_->inFinalState())
		cancelButton_->hide();
	else
		cancelButton_->show();
}

void SGMSampleChamberVacuumMoveActionDialog::onOKButtonClicked()
{
	// The OK button is visible when the action needs user input, or when it has
	// reached a final state and it's time to close the dialog.

	if (action_) {

		if (action_->inFinalState()) {
			done(QDialog::Accepted);

		} else {

			// If the currently executing action is a timeout action and the OK button has
			// been clicked, the user has indicated they are ready to proceed.

			AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(action_->currentSubAction());

			if (timeoutAction)
				timeoutAction->proceed();
		}
	}
}

void SGMSampleChamberVacuumMoveActionDialog::onCancelButtonClicked()
{
	// Cancel the action, and then close the dialog.

	if (action_)
		action_->cancel();

	done(QDialog::Rejected);
}
