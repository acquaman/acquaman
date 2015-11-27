#include "SGMSampleChamberVacuumMoveActionView.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"
#include "actions3/actions/AMTimeoutAction.h"

SGMSampleChamberVacuumMoveActionStatusView::SGMSampleChamberVacuumMoveActionStatusView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;
	actionStatus_ = -1;

	// Create UI elements.

	statusLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
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

		if (action_) {
			setActionStatus(-1);
			disconnect( action_, 0, this, 0 );
		}

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(stateChanged(int,int)), this, SLOT(onActionStateChanged(int,int)) );
			setActionStatus(action_->state());
		}
	}
}

void SGMSampleChamberVacuumMoveActionStatusView::setActionStatus(int newStatus)
{
	if (actionStatus_ != newStatus) {
		actionStatus_ = newStatus;

		refresh();
	}
}

void SGMSampleChamberVacuumMoveActionStatusView::updateStatusLabel()
{
	QString newText = stateToString(actionStatus_);
	statusLabel_->setText(newText);
}

void SGMSampleChamberVacuumMoveActionStatusView::onActionStateChanged(int oldState, int newState)
{
	Q_UNUSED(oldState)

	if ((newState == AMAction3::Cancelled) || (newState == AMAction3::Succeeded) || (newState == AMAction3::Failed) || (newState == AMAction3::Skipping))
		action_ = 0;

	setActionStatus(newState);
}

QString SGMSampleChamberVacuumMoveActionStatusView::stateToString(int state) const
{
	switch (state) {
	case -1:
		return "No action set.";
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
			connect( action_, SIGNAL(stateChanged(int,int)), this, SLOT(onActionStateChanged(int,int)) );
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateDescriptionView()) );
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateCountdownView()) );
		}

		refresh();
	}
}

void SGMSampleChamberVacuumMoveActionView::updateDescriptionView()
{
	// Update the description view's action info and visibility.

	AMActionInfo3 *info = 0;
	bool showView = false;

	if (action_ && action_->currentSubAction())
		info = action_->currentSubAction()->info();

	if (action_ && action_->inFinalState())
		showView = false;
	else
		showView = true;

	descriptionView_->setInfo(info);
	descriptionView_->setVisible(showView);
}

void SGMSampleChamberVacuumMoveActionView::updateStatusView()
{
	// Update the view's visibility.

	bool showView = false;

	if (action_ && action_->inFinalState())
		showView = true;

	statusView_->setVisible(showView);
}

void SGMSampleChamberVacuumMoveActionView::updateCountdownView()
{
	// Update the countdown's action and visibility.

	AMAction3 *action = 0;
	bool showView = false;

	if (action_ && action_->currentSubAction()) {
		AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(action_->currentSubAction());

		if (timeoutAction) {
			action = timeoutAction;
			showView = true;
		}
	}

	countdownView_->setAction(action);
	countdownView_->setVisible(showView);
}

void SGMSampleChamberVacuumMoveActionView::onActionStateChanged(int oldState, int newState)
{
	Q_UNUSED(oldState)

	if ((newState == AMAction3::Cancelled) || (newState == AMAction3::Succeeded) || (newState == AMAction3::Failed) || (newState == AMAction3::Skipping))
		action_ = 0;

	refresh();
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

	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton(okButton_, QDialogButtonBox::AcceptRole);
	buttonBox->addButton(cancelButton_, QDialogButtonBox::RejectRole);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(moveView_);
	layout->addWidget(buttonBox);

	setLayout(layout);

	// Make connections.

	connect( buttonBox, SIGNAL(accepted()), this, SLOT(onOKButtonClicked()) );
	connect( buttonBox, SIGNAL(rejected()), this, SLOT(onCancelButtonClicked()) );

	// Current settings.

	setAction(action);

	refresh();
}

SGMSampleChamberVacuumMoveActionDialog::~SGMSampleChamberVacuumMoveActionDialog()
{

}

void SGMSampleChamberVacuumMoveActionDialog::clear()
{
	moveView_->clear();

	okButton_->show();
	cancelButton_->hide();
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
			connect( action_, SIGNAL(stateChanged(int,int)), this, SLOT(onActionStateChanged(int,int)) );
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateButtons()) );
		}

		refresh();
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

	if (action_ && action_->currentSubAction()) {
		AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(action_->currentSubAction());

		if (timeoutAction)
			showButton = true;

	} else {
		showButton = true;
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

	if (action_ && !action_->inFinalState())
		cancelButton_->show();
	else
		cancelButton_->hide();
}

void SGMSampleChamberVacuumMoveActionDialog::onActionStateChanged(int oldState, int newState)
{
	Q_UNUSED(oldState)

	if ((newState == AMAction3::Cancelled) || (newState == AMAction3::Succeeded) || (newState == AMAction3::Failed) || (newState == AMAction3::Skipping))
		action_ = 0;

	updateButtons();
}

void SGMSampleChamberVacuumMoveActionDialog::onOKButtonClicked()
{
	// The OK button is visible when the action needs user input, or when it has
	// reached a final state and it's time to close the dialog.

	if (action_ && !action_->inFinalState()) {
		AMAction3 *subAction = action_->currentSubAction();
		AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(subAction);

		if (timeoutAction)
			timeoutAction->proceed();

	} else {
		done(QDialog::Accepted);
	}
}

void SGMSampleChamberVacuumMoveActionDialog::onCancelButtonClicked()
{
	// Cancel the action, and then close the dialog.

	if (action_)
		action_->cancel();

	done(QDialog::Rejected);
}
