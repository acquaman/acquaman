#include "SGMSampleChamberVacuumMoveActionView.h"

SGMSampleChamberVacuumMoveActionView::SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.


	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();

	setLayout(layout);

	// Current settings.

	setAction(action);
}

SGMSampleChamberVacuumMoveActionView::~SGMSampleChamberVacuumMoveActionView()
{

}

void SGMSampleChamberVacuumMoveActionView::clear()
{

}

void SGMSampleChamberVacuumMoveActionView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

}

void SGMSampleChamberVacuumMoveActionView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {
		action_ = newAction;
		refresh();

		emit actionChanged(action_);
	}
}
