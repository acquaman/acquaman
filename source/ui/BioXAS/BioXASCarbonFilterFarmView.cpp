#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

	// Create UI elements.

	upstreamPositionEditor_ = new AMExtendedControlEditor(0);
	upstreamPositionEditor_->setTitle("Position");

	upstreamStatusEditor_ = new AMExtendedControlEditor(0, 0, true);
	upstreamStatusEditor_->setTitle("Status");

	downstreamPositionEditor_ = new AMExtendedControlEditor(0);
	downstreamPositionEditor_->setTitle("Position");

	downstreamStatusEditor_ = new AMExtendedControlEditor(0, 0, true);
	downstreamStatusEditor_->setTitle("Status");

	// Create and set layouts.

	QVBoxLayout *upstreamTestingLayout = new QVBoxLayout();
	upstreamTestingLayout->addWidget(upstreamPositionEditor_);
	upstreamTestingLayout->addWidget(upstreamPositionStatusEditor_);

	QGroupBox *upstreamTesting = new QGroupBox("Upstream controls");
	upstreamTesting->setLayout(upstreamTestingLayout);

	QVBoxLayout *downstreamTestingLayout = new QVBoxLayout();
	downstreamTestingLayout->addWidget(downstreamPositionEditor_);
	downstreamTestingLayout->addWidget(downstreamPositionStatusEditor_);

	QGroupBox *downstreamTesting = new QGroupBox("Downstream controls");
	downstreamTesting->setLayout(downstreamTestingLayout);

	QVBoxLayout *layout = new QVBoxLayout();
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

			upstreamPositionEditor_->setControl(0);
			upstreamPositionStatusEditor_->setControl(0);

			downstreamPositionEditor_->setControl(0);
			downstreamStatusEditor_->setControl(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Update UI elements.

			upstreamPositionEditor_->setControl(filterFarm_->upstreamPositionControl());
			upstreamPositionStatusEditor_->setControl(filterFarm_->upstreamStatusControl());

			downstreamPositionEditor_->setControl(filterFarm_->downstreamPositionControl());
			downstreamStatusEditor_->setControl(filterFarm_->downstreamStatusControl());
		}

		emit filterFarmChanged(filterFarm_);
	}
}
