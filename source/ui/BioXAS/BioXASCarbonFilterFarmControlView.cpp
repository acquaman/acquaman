#include "BioXASCarbonFilterFarmControlView.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASCarbonFilterFarmControlView::BioXASCarbonFilterFarmControlView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent) :
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

	setFilterFarm(filterFarm);
	refresh();
}

BioXASCarbonFilterFarmControlView::~BioXASCarbonFilterFarmControlView()
{

}

void BioXASCarbonFilterFarmControlView::clear()
{
	filterEditor_->setControl(0);
	upstreamActuatorView_->setActuator(0);
	downstreamActuatorView_->setActuator(0);
}

void BioXASCarbonFilterFarmControlView::refresh()
{
	// Clear the view.

	clear();

	// Update the view with the appropriate controls.

	if (filterFarm_) {
		filterEditor_->setControl(filterFarm_->filter());
		upstreamActuatorView_->setActuator(filterFarm_->upstreamActuator());
		downstreamActuatorView_->setActuator(filterFarm_->downstreamActuator());
	}
}

void BioXASCarbonFilterFarmControlView::setFilterFarm(BioXASCarbonFilterFarmControl *newControl)
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

		emit filterFarmChanged(filterFarm_);
	}
}
