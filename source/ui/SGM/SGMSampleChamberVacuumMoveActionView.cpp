#include "SGMSampleChamberVacuumMoveActionView.h"

SGMSampleChamberVacuumMoveSubActionView::SGMSampleChamberVacuumMoveSubActionView(AMAction3 *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	descriptionLabel_ = new QLabel();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(descriptionLabel_);

	setLayout(layout);

	// Current settings.

	setAction(action);

	refresh();
}

SGMSampleChamberVacuumMoveSubActionView::~SGMSampleChamberVacuumMoveSubActionView()
{

}

void SGMSampleChamberVacuumMoveSubActionView::clear()
{
	descriptionLabel_->clear();
}

void SGMSampleChamberVacuumMoveSubActionView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateDescriptionLabel();
}

void SGMSampleChamberVacuumMoveSubActionView::setAction(AMAction3 *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_) {
			connect( action_->info(), SIGNAL(infoChanged()), this, SLOT(updateDescriptionLabel()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void SGMSampleChamberVacuumMoveSubActionView::updateDescriptionLabel()
{
	if (action_)
		descriptionLabel_->setText(action_->info()->longDescription());
	else
		descriptionLabel_->clear();
}

SGMSampleChamberVacuumMoveActionView::SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	okButton_ = new QPushButton("Ok");

	cancelButton_ = new QPushButton("Cancel");

	// Create and set layouts.

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton_);
	buttonLayout->addWidget(cancelButton_);

	subActionViewsLayout_ = new QVBoxLayout();
	subActionViewsLayout_->setMargin(0);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(subActionViewsLayout_);
	layout->addLayout(buttonLayout);

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
	// Remove and delete all subaction views.

	removeAllSubActionViews();
	deleteAllSubActionViews();
}

void SGMSampleChamberVacuumMoveActionView::refresh()
{
	// Clear the view.

	clear();

	// Create view for each of the action's subactions.
	// Add view to the list of views and the views layout.

	if (action_) {

		for (int i = 0, count = action_->subActionCount(); i < count; i++) {
			AMAction3 *subAction = action_->subActionAt(i);
			QWidget *subActionView = createSubActionView(subAction);

			addSubActionView(subActionView);
		}
	}

	// Update view elements.

	updateSubActionView();
	updateOKButton();
	updateCancelButton();
}

void SGMSampleChamberVacuumMoveActionView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_) {
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateSubActionView()) );
			connect( action_, SIGNAL(currentSubActionChanged(int)), this, SLOT(updateOKButton()) );
		}

		refresh();

		emit actionChanged(action_);
	}
}

void SGMSampleChamberVacuumMoveActionView::updateSubActionView()
{
	if (action_) {

		// Identify the currently executing subaction and its corresponding widget.

		int currentIndex = action_->currentSubActionIndex();
		QWidget *actionView = viewForActionIndex(currentIndex);

		// Hide all subaction views.

		hideAllSubActionViews();

		// Show the view for the given action.

		if (actionView)
			actionView->show();
	}
}

void SGMSampleChamberVacuumMoveActionView::updateOKButton()
{
	if (action_) {

		// The OK button is only shown when the currently executing action is of
		// type AMTimeoutAction--we're waiting for a user to say they've taken a
		// necessary step. Otherwise, the button is hidden.

		AMAction3 *currentAction = action_->currentSubAction();

		AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(currentAction);

		if (timeoutAction)
			okButton_->show();
		else
			okButton_->hide();

	} else {

		okButton_->hide();
	}
}

void SGMSampleChamberVacuumMoveActionView::updateCancelButton()
{
	// The cancel button should always be visible, and enabled if the vacuum
	// move action is valid.

	if (action_)
		cancelButton_->setEnabled(true);
	else
		cancelButton_->setEnabled(false);
}

QWidget* SGMSampleChamberVacuumMoveActionView::createSubActionView(AMAction3 *subAction)
{
	QWidget *result = 0;

	if (subAction) {

		// If the given subaction is a timeout action, create the view specific for
		// that type of action. Otherwise, return the generic subaction view.

		AMTimeoutAction *timeoutAction = qobject_cast<AMTimeoutAction*>(subAction);

		if (timeoutAction)
			result = new AMTimeoutActionView(timeoutAction);
		else
			result = new SGMSampleChamberVacuumMoveSubActionView(subAction);
	}

	return result;
}

void SGMSampleChamberVacuumMoveActionView::addSubActionView(QWidget *newView)
{
	if (newView && !subActionViews_.contains(newView)) {

		// Add the view to the list of subaction views and to the
		// main layout.

		subActionViews_.append(newView);
		subActionViewsLayout_->addWidget(newView);
	}
}

void SGMSampleChamberVacuumMoveActionView::hideAllSubActionViews()
{
	// Iterate through the views list, hiding all valid widgets.

	foreach (QWidget *view, subActionViews_) {
		if (view)
			view->hide();
	}
}

void SGMSampleChamberVacuumMoveActionView::removeAllSubActionViews()
{
	// Iterate through the views list, remove each instance from the views
	// layout.

	foreach (QWidget *view, subActionViews_) {
		if (view)
			subActionViewsLayout_->removeWidget(view);
	}
}

void SGMSampleChamberVacuumMoveActionView::deleteAllSubActionViews()
{
	// Iterate through the views list, removing each instance from
	// the list of views and deleting it.

	foreach (QWidget *view, subActionViews_) {
		subActionViews_.removeOne(view);

		if (view)
			view->deleteLater();
	}
}

QWidget* SGMSampleChamberVacuumMoveActionView::viewForActionIndex(int index)
{
	QWidget *result = 0;

	// The views are indexed by action index.

	if (index >= 0 && index < subActionViews_.count())
		result = subActionViews_.at(index);

	return result;
}
