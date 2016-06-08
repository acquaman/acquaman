#include "BioXASCarbonFilterFarmActuatorView.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"
#include "ui/CLS/CLSControlEditor.h"

BioXASCarbonFilterFarmActuatorView::BioXASCarbonFilterFarmActuatorView(BioXASCarbonFilterFarmActuator *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	actuator_ = 0;

	// Create UI elements.

	filterEditor_ = new CLSControlEditor(0);
	filterEditor_->setTitle("Filter");

	windowEditor_ = new CLSControlEditor(0);
	windowEditor_->setTitle("Window");

	motorEditor_ = new CLSControlEditor(0);
	motorEditor_->setTitle("Position");
	motorEditor_->setMinimumWidth(150);

	positionStatusEditor_ = new CLSControlEditor(0);
	positionStatusEditor_->setTitle("Status");
	positionStatusEditor_->setMinimumWidth(150);

	// Create and set layouts.

	QHBoxLayout *positionLayout = new QHBoxLayout();
	positionLayout->setMargin(0);
	positionLayout->addWidget(motorEditor_);
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
	updateFilterEditor();
	updateWindowEditor();
	updateMotorEditor();
	updatePositionStatusEditor();
}

void BioXASCarbonFilterFarmActuatorView::setControl(BioXASCarbonFilterFarmActuator *newControl)
{
	if (actuator_ != newControl) {

		if (actuator_)
			disconnect( actuator_, 0, this, 0 );

		actuator_ = newControl;

		if (actuator_) {
			connect( actuator_, SIGNAL(motorChanged(CLSMAXvMotor*)), this, SLOT(updateMotorEditor()) );
			connect( actuator_, SIGNAL(positionStatusChanged(AMControl*)), this, SLOT(updatePositionStatusEditor()) );
		}

		refresh();

		emit controlChanged(actuator_);
	}
}

void BioXASCarbonFilterFarmActuatorView::updateFilterEditor()
{
	filterEditor_->setControl(0);

	if (actuator_)
		filterEditor_->setControl(actuator_->filter());
}

void BioXASCarbonFilterFarmActuatorView::updateWindowEditor()
{
	windowEditor_->setControl(0);

	if (actuator_)
		windowEditor_->setControl(actuator_->window());
}

void BioXASCarbonFilterFarmActuatorView::updateMotorEditor()
{
	motorEditor_->setControl(0);

	if (actuator_)
		motorEditor_->setControl(actuator_->motor());
}

void BioXASCarbonFilterFarmActuatorView::updatePositionStatusEditor()
{
	positionStatusEditor_->setControl(0);

	if (actuator_)
		positionStatusEditor_->setControl(actuator_->positionStatus());
}
