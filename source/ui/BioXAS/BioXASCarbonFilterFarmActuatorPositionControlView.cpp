#include "BioXASCarbonFilterFarmActuatorPositionControlView.h"

BioXASCarbonFilterFarmActuatorPositionControlView::BioXASCarbonFilterFarmActuatorPositionControlView(BioXASCarbonFilterFarmActuatorPositionControl *positionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	position_ = 0;

	// Create UI elements.

	positionEditor_ = new AMExtendedControlEditor(0);
	positionEditor_->setTitle("Position");

	positionStatusEditor_ = new AMExtendedControlEditor(0);
	positionStatusEditor_->setTitle("Status");
	positionStatusEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(positionEditor_);
	layout->addWidget(positionStatusEditor_);

	setLayout(layout);

	// Current settings.

	setPosition(positionControl);
	refresh();
}

BioXASCarbonFilterFarmActuatorPositionControlView::~BioXASCarbonFilterFarmActuatorPositionControlView()
{

}

void BioXASCarbonFilterFarmActuatorPositionControlView::clear()
{
	positionEditor_->setControl(0);
	positionStatusEditor_->setControl(0);
}

void BioXASCarbonFilterFarmActuatorPositionControlView::refresh()
{
	refreshPositionEditor();
	refreshPositionStatusEditor();
}

void BioXASCarbonFilterFarmActuatorPositionControlView::setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	if (position_ != newControl) {

		if (position_)
			disconnect( position_, 0, this, 0 );

		position_ = newControl;

		if (position_) {
			connect( position_, SIGNAL(positionControlChanged(AMControl*)), this, SLOT(refreshPositionEditor()) );
			connect( position_, SIGNAL(statusControlChanged(AMControl*)), this, SLOT(refreshPositionStatusEditor()) );
		}

		refresh();

		emit positionChanged(position_);
	}
}

void BioXASCarbonFilterFarmActuatorPositionControlView::refreshPositionEditor()
{
	positionEditor_->setControl(0);

	if (position_)
		positionEditor_->setControl(position_->positionControl());
}

void BioXASCarbonFilterFarmActuatorPositionControlView::refreshPositionStatusEditor()
{
	positionStatusEditor_->setControl(0);

	if (position_)
		positionStatusEditor_->setControl(position_->statusControl());
}
