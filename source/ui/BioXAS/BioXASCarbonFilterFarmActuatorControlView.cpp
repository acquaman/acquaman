#include "BioXASCarbonFilterFarmActuatorControlView.h"

BioXASCarbonFilterFarmActuatorControlView::BioXASCarbonFilterFarmActuatorControlView(BioXASCarbonFilterFarmActuatorControl *actuator, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	actuator_ = 0;

	// Create UI elements.

	filterEditor_ = new AMExtendedControlEditor(0);
	filterEditor_->setTitle("Filter");
	filterEditor_->setUnits("mm");

	windowEditor_ = new AMExtendedControlEditor(0);
	windowEditor_->setTitle("Window");
	windowEditor_->setNoUnitsBox(true);

	positionEditor_ = new AMExtendedControlEditor(0);
	positionEditor_->setTitle("Position");

	positionStatusEditor_ = new AMExtendedControlEditor(0);
	positionStatusEditor_->setTitle("Status");
	positionStatusEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *positionLayout = new QHBoxLayout();
	positionLayout->setMargin(0);
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
	refreshFilterEditor();
	refreshWindowEditor();
	refreshPositionEditor();
	refreshPositionStatusEditor();
}

void BioXASCarbonFilterFarmActuatorControlView::setActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (actuator_ != newControl) {

		if (actuator_)
			disconnect( actuator_, 0, this, 0 );

		actuator_ = newControl;

		if (actuator_) {
			connect( actuator_, SIGNAL(filterChanged(BioXASCarbonFilterFarmActuatorFilterControl*)), this, SLOT(refreshFilterEditor()) );
			connect( actuator_, SIGNAL(windowChanged(BioXASCarbonFilterFarmActuatorWindowControl*)), this, SLOT(refreshWindowEditor()) );
			connect( actuator_, SIGNAL(positionChanged(AMPVControl*)), this, SLOT(refreshPositionEditor()) );
			connect( actuator_, SIGNAL(positionStatusChanged(AMReadOnlyPVControl*)), this, SLOT(refreshPositionStatusEditor()) );
		}

		refresh();

		emit actuatorChanged(actuator_);
	}
}

void BioXASCarbonFilterFarmActuatorControlView::refreshFilterEditor()
{
	filterEditor_->setControl(0);

	if (actuator_)
		filterEditor_->setControl(actuator_->filter());
}

void BioXASCarbonFilterFarmActuatorControlView::refreshWindowEditor()
{
	windowEditor_->setControl(0);

	if (actuator_)
		windowEditor_->setControl(actuator_->window());
}

void BioXASCarbonFilterFarmActuatorControlView::refreshPositionEditor()
{
	positionEditor_->setControl(0);

	if (actuator_)
		positionEditor_->setControl(actuator_->position());
}

void BioXASCarbonFilterFarmActuatorControlView::refreshPositionStatusEditor()
{
	positionStatusEditor_->setControl(0);

	if (actuator_)
		positionStatusEditor_->setControl(actuator_->positionStatus());
}
