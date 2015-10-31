#include "BioXASCarbonFilterFarmActuatorControlView.h"

BioXASCarbonFilterFarmActuatorControlView::BioXASCarbonFilterFarmActuatorControlView(BioXASCarbonFilterFarmActuatorControl *actuator, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	actuator_ = 0;

	// Create UI elements.

	filterEditor_ = new AMExtendedControlEditor(0);
	filterEditor_->setTitle("Filter");

	windowEditor_ = new AMExtendedControlEditor(0);
	windowEditor_->setTitle("Window");

	positionEditor_ = new AMExtendedControlEditor(0);
	positionEditor_->setTitle("Position");

	positionStatusEditor_ = new AMExtendedControlEditor(0);
	positionStatusEditor_->setTitle("Position status");
	positionStatusEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *positionLayout = new QHBoxLayout();
	positionLayout->addWidget(positionEditor_);
	positionLayout->addWidget(positionStatusEditor_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(filterEditor_);
	layout->addWidget(windowEditor_);
	layout->addLayout(positionLayout);

	setLayout(layout);

	// Current settings.

	setActuator(actuator);
	refresh();
}

BioXASCarbonFilterFarmActuatorControlView::~BioXASCarbonFilterFarmActuatorControlView()
{

}

void BioXASCarbonFilterFarmActuatorControlView::clear()
{
	filterEditor_->setControl(0);
	windowEditor_->setControl(0);
	positionEditor_->setControl(0);
	positionStatusEditor_->setControl(0);
}

void BioXASCarbonFilterFarmActuatorControlView::refresh()
{
	// Clear the view.

	clear();

	// Update the controls being viewed.

	if (actuator_) {
		filterEditor_->setControl(actuator_->filter());
		windowEditor_->setControl(actuator_->window());

		if (actuator_->position()) {
			positionEditor_->setControl(actuator_->position()->positionControl());
			positionStatusEditor_->setControl(actuator_->position()->statusControl());
		}
	}
}

void BioXASCarbonFilterFarmActuatorControlView::setActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (actuator_ != newControl) {

		if (actuator_)
			disconnect( actuator_, 0, this, 0 );

		actuator_ = newControl;

		if (actuator_) {
			connect( actuator_, SIGNAL(positionChanged(AMControl*)), this, SLOT(refresh()) );
			connect( actuator_, SIGNAL(windowChanged(AMControl*)), this, SLOT(refresh()) );
			connect( actuator_, SIGNAL(filterChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit actuatorChanged(actuator_);
	}
}
