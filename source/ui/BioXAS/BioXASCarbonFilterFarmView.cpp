#include "BioXASCarbonFilterFarmView.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmActuatorControlView.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	filterFarm_ = 0;

	// Create UI elements.

	filterEditor_ = new BioXASControlEditor(0);
	filterEditor_->setTitle("Effective filter");

	upstreamActuatorView_ = new BioXASCarbonFilterFarmActuatorControlView(0);

	downstreamActuatorView_ = new BioXASCarbonFilterFarmActuatorControlView(0);

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
	upstreamActuatorView_->setActuator(0);
	downstreamActuatorView_->setActuator(0);

	// Update the view with the appropriate controls.

	if (filterFarm_) {
		filterEditor_->setControl(filterFarm_->filter());
		upstreamActuatorView_->setActuator(filterFarm_->upstreamActuator());
		downstreamActuatorView_->setActuator(filterFarm_->downstreamActuator());
	}
}

void BioXASCarbonFilterFarmView::setControl(BioXASCarbonFilterFarm *newControl)
{
	if (filterFarm_ != newControl) {

		if (filterFarm_)
			disconnect( filterFarm_, 0, this, 0 );

		filterFarm_ = newControl;

		if (filterFarm_) {
			connect( filterFarm_, SIGNAL(filterChanged(AMControl*)), this, SLOT(refresh()) );
			connect( filterFarm_, SIGNAL(upstreamActuatorChanged(AMControl*)), this, SLOT(refresh()) );
			connect( filterFarm_, SIGNAL(downstreamActuatorChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(filterFarm_);
	}
}
