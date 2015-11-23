#include "SGMSampleChamberVacuumMoveActionView.h"

SGMSampleChamberVacuumMoveActionLEDView::SGMSampleChamberVacuumMoveActionLEDView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create and set layout.

	layout_ = new QVBoxLayout();

	setLayout(layout_);

	// Current settings.

	setAction(action);
}

SGMSampleChamberVacuumMoveActionLEDView::~SGMSampleChamberVacuumMoveActionLEDView()
{

}

void SGMSampleChamberVacuumMoveActionLEDView::clear()
{
	foreach (QLabel *led, leds_) {
		layout_->removeWidget(led);
		led->deleteLater();
	}
}

void SGMSampleChamberVacuumMoveActionLEDView::refresh()
{
	// Clear the view.

	clear();

	// Create an led for each subaction, colored according to the subaction running
	// status.

	if (action_) {

		for (int i = 0, count = action_->subActionCount(); i < count; i++) {
			AMAction3 *subAction = action_->subActionAt(i);

			if (subAction) {

				AMAction3::State subActionState = subAction->state();
				QLabel *led = new QLabel();

				if (subActionState == AMAction3::Succeeded)
					led->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
				else if (subActionState == AMAction3::Failed)
					led->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
				else if (subActionState == AMAction3::Running)
					led->setPixmap(QPixmap(":/22x22/yellowLEDOn.png"));
				else
					led->setPixmap(QPixmap(":/22x22/yellowLEDOff.png"));

				leds_.append(led);
				layout_->addWidget(led);
			}
		}
	}
}

void SGMSampleChamberVacuumMoveActionLEDView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {

		if (action_)
			disconnect( action_, 0, this, 0 );

		action_ = newAction;

		if (action_)
			connect( action_, SIGNAL(progressChanged(double,double)), this, SLOT(refresh()) );

		refresh();

		emit actionChanged(action_);
	}
}

SGMSampleChamberVacuumMoveActionView::SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	action_ = 0;

	// Create UI elements.

	ledView_ = new SGMSampleChamberVacuumMoveActionView(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(ledView_);

	setLayout(layout);

	// Current settings.

	setAction(action);
}

SGMSampleChamberVacuumMoveActionView::~SGMSampleChamberVacuumMoveActionView()
{

}

void SGMSampleChamberVacuumMoveActionView::clear()
{
	ledView_->setAction(0);
}

void SGMSampleChamberVacuumMoveActionView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	ledView_->setAction(action_);
}

void SGMSampleChamberVacuumMoveActionView::setAction(SGMSampleChamberVacuumMoveAction *newAction)
{
	if (action_ != newAction) {
		action_ = newAction;
		refresh();

		emit actionChanged(action_);
	}
}
