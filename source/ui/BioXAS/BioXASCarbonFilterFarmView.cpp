#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

	filterEditor_ = new AMExtendedControlEditor(0);

	upstreamEditor_ = new AMExtendedControlEditor(0);
	upstreamEditor_->setTitle("Actuator control");

	upstreamPositionEditor_ = new AMExtendedControlEditor(0);
	upstreamPositionEditor_->setTitle("Position");

	upstreamPositionStatusEditor_ = new AMExtendedControlEditor(0);
	upstreamPositionStatusEditor_->setTitle("Position status");

	downstreamEditor_ = new AMExtendedControlEditor(0);
	downstreamEditor_->setTitle("Actuator control");

	downstreamPositionEditor_ = new AMExtendedControlEditor(0);
	downstreamPositionEditor_->setTitle("Position");

	downstreamPositionStatusEditor_ = new AMExtendedControlEditor(0);
	downstreamPositionStatusEditor_->setTitle("Position status");

	// Create and set layouts.

	QVBoxLayout *upstreamTestingLayout = new QVBoxLayout();
	upstreamTestingLayout->addWidget(upstreamEditor_);
	upstreamTestingLayout->addWidget(upstreamPositionEditor_);
	upstreamTestingLayout->addWidget(upstreamPositionStatusEditor_);

	QGroupBox *upstreamTesting = new QGroupBox("Testing - upstream sub-controls");
	upstreamTesting->setLayout(upstreamTestingLayout);

	QVBoxLayout *downstreamTestingLayout = new QVBoxLayout();
	downstreamTestingLayout->addWidget(downstreamEditor_);
	downstreamTestingLayout->addWidget(downstreamPositionEditor_);
	downstreamTestingLayout->addWidget(downstreamPositionStatusEditor_);

	QGroupBox *downstreamTesting = new QGroupBox("Testing - downstream sub-controls");
	downstreamTesting->setLayout(downstreamTestingLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(filterEditor_);
	layout->addWidget(upstreamTesting);
	layout->addWidget(downstreamTesting);

	setLayout(layout);

	// Current settings.

	setFilterFarm(filterFarm);
}

BioXASCarbonFilterFarmView::~BioXASCarbonFilterFarmView()
{

}

void BioXASCarbonFilterFarmView::setFilterFarm(BioXASCarbonFilterFarm *newFilterFarm)
{
	if (filterFarm_ != newFilterFarm) {

		if (filterFarm_) {

			// Clear UI elements.

			filterEditor_->setControl(0);

			upstreamEditor_->setControl(0);
			upstreamPositionEditor_->setControl(0);
			upstreamPositionStatusEditor_->setControl(0);

			downstreamEditor_->setControl(0);
			downstreamPositionEditor_->setControl(0);
			downstreamPositionStatusEditor_->setControl(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Update UI elements.

			filterEditor_->setControl(filterFarm_->filterControl());

			upstreamEditor_->setControl(filterFarm_->upstreamActuatorControl());
			upstreamPositionEditor_->setControl(filterFarm_->upstreamPositionControl());
			upstreamPositionStatusEditor_->setControl(filterFarm_->upstreamStatusControl());

			downstreamEditor_->setControl(filterFarm_->downstreamActuatorControl());
			downstreamPositionEditor_->setControl(filterFarm_->downstreamPositionControl());
			downstreamPositionStatusEditor_->setControl(filterFarm_->downstreamStatusControl());
		}

		emit filterFarmChanged(filterFarm_);
	}
}
