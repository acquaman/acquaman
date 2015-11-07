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

	positionView_ = new BioXASCarbonFilterFarmActuatorPositionControlView(0);
	positionView_->setMinimumWidth(400);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(filterEditor_);
	layout->addWidget(windowEditor_);
	layout->addWidget(positionView_);

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
	positionView_->setPosition(0);
}

void BioXASCarbonFilterFarmActuatorControlView::refresh()
{
	refreshFilterEditor();
	refreshWindowEditor();
	refreshPositionView();
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
			connect( actuator_, SIGNAL(positionChanged(BioXASCarbonFilterFarmActuatorPositionControl*)), this, SLOT(refreshPositionView()) );
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

void BioXASCarbonFilterFarmActuatorControlView::refreshPositionView()
{
	positionView_->setPosition(0);

	if (actuator_)
		positionView_->setPosition(actuator_->position());
}
