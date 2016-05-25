#include "BioXASCarbonFilterFarmView.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "ui/CLS/CLSControlEditor.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmActuatorView.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	filterFarm_ = 0;

	// Create UI elements.

	filterEditor_ = new CLSControlEditor(0);
	filterEditor_->setTitle("Effective filter");

	upstreamActuatorView_ = new BioXASCarbonFilterFarmActuatorView(0);

	downstreamActuatorView_ = new BioXASCarbonFilterFarmActuatorView(0);

	// Create and set layouts.

	QVBoxLayout *upstreamLayout = new QVBoxLayout();
	upstreamLayout->setMargin(0);
	upstreamLayout->addWidget(upstreamActuatorView_);

	QGroupBox *upstreamBox = new QGroupBox();
	upstreamBox->setTitle("Upstream Actuator");
	upstreamBox->setFlat(true);
	upstreamBox->setLayout(upstreamLayout);

	QVBoxLayout *downstreamLayout = new QVBoxLayout();
	downstreamLayout->setMargin(0);
	downstreamLayout->addWidget(downstreamActuatorView_);

	QGroupBox *downstreamBox = new QGroupBox();
	downstreamBox->setTitle("Downstream Actuator");
	downstreamBox->setFlat(true);
	downstreamBox->setLayout(downstreamLayout);

	QHBoxLayout *actuatorsLayout = new QHBoxLayout();
	actuatorsLayout->addWidget(upstreamBox);
	actuatorsLayout->addWidget(downstreamBox);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(filterEditor_);
	layout->addLayout(actuatorsLayout);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASCarbonFilterFarmView::~BioXASCarbonFilterFarmView()
{

}

void BioXASCarbonFilterFarmView::refresh()
{
	// Clear the view.

	filterEditor_->setControl(0);
	upstreamActuatorView_->setControl(0);
	downstreamActuatorView_->setControl(0);

	// Update the view with the appropriate controls.

	if (filterFarm_) {
		filterEditor_->setControl(filterFarm_->filter());
		upstreamActuatorView_->setControl(filterFarm_->upstreamActuator());
		downstreamActuatorView_->setControl(filterFarm_->downstreamActuator());
	}
}

void BioXASCarbonFilterFarmView::setControl(BioXASCarbonFilterFarm *newControl)
{
	if (filterFarm_ != newControl) {
		filterFarm_ = newControl;
		refresh();

		emit controlChanged(filterFarm_);
	}
}
