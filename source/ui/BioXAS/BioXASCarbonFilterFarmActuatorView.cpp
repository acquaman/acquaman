#include "BioXASCarbonFilterFarmActuatorView.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASCarbonFilterFarmActuatorView::BioXASCarbonFilterFarmActuatorView(BioXASCarbonFilterFarmActuator *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	actuator_ = 0;

	// Create UI elements.

	filterEditor_ = new BioXASControlEditor(0);
	filterEditor_->setTitle("Filter");

	windowEditor_ = new BioXASControlEditor(0);
	windowEditor_->setTitle("Window");

	positionEditor_ = new BioXASControlEditor(0);
	positionEditor_->setTitle("Position");
	positionEditor_->setMinimumWidth(150);

	positionStatusEditor_ = new BioXASControlEditor(0);
	positionStatusEditor_->setTitle("Status");
	positionStatusEditor_->setMinimumWidth(100);

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

	setControl(control);
	refresh();
}

BioXASCarbonFilterFarmActuatorView::~BioXASCarbonFilterFarmActuatorView()
{

}

void BioXASCarbonFilterFarmActuatorView::refresh()
{
	// Refresh UI elements.

	refreshFilterEditor();
	refreshWindowEditor();
	refreshPositionEditor();
	refreshPositionStatusEditor();
}

void BioXASCarbonFilterFarmActuatorView::setControl(BioXASCarbonFilterFarmActuator *newControl)
{
	if (actuator_ != newControl) {

		if (actuator_)
			disconnect( actuator_, 0, this, 0 );

		actuator_ = newControl;

		if (actuator_) {
			connect( actuator_, SIGNAL(filterChanged(AMControl*)), this, SLOT(refreshFilterEditor()) );
			connect( actuator_, SIGNAL(windowChanged(AMControl*)), this, SLOT(refreshWindowEditor()) );
			connect( actuator_, SIGNAL(positionChanged(AMControl*)), this, SLOT(refreshPositionEditor()) );
			connect( actuator_, SIGNAL(positionStatusChanged(AMControl*)), this, SLOT(refreshPositionStatusEditor()) );
		}

		refresh();

		emit controlChanged(actuator_);
	}
}

void BioXASCarbonFilterFarmActuatorView::refreshFilterEditor()
{
	filterEditor_->setControl(0);

	if (actuator_)
		filterEditor_->setControl(actuator_->filter());
}

void BioXASCarbonFilterFarmActuatorView::refreshWindowEditor()
{
	windowEditor_->setControl(0);

	if (actuator_)
		windowEditor_->setControl(actuator_->window());
}

void BioXASCarbonFilterFarmActuatorView::refreshPositionEditor()
{
	positionEditor_->setControl(0);

	if (actuator_)
		positionEditor_->setControl(actuator_->position());
}

void BioXASCarbonFilterFarmActuatorView::refreshPositionStatusEditor()
{
	positionStatusEditor_->setControl(0);

	if (actuator_)
		positionStatusEditor_->setControl(actuator_->positionStatus());
}
