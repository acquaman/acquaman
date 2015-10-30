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
	upstreamTestingLayout->addWidget(upstreamStatusEditor_);

	QGroupBox *upstreamTesting = new QGroupBox("Upstream controls");
	upstreamTesting->setLayout(upstreamTestingLayout);

	QVBoxLayout *downstreamTestingLayout = new QVBoxLayout();
	downstreamTestingLayout->addWidget(downstreamPositionEditor_);
	downstreamTestingLayout->addWidget(downstreamStatusEditor_);

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

void BioXASCarbonFilterFarmView::clear()
{
	upstreamPositionEditor_->setControl(0);
	upstreamStatusEditor_->setControl(0);

	downstreamPositionEditor_->setControl(0);
	downstreamStatusEditor_->setControl(0);
}

void BioXASCarbonFilterFarmView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	if (filterFarm_) {

		if (filterFarm_->upstreamActuator() && filterFarm_->upstreamActuator()->position()) {
			upstreamPositionEditor_->setControl(filterFarm_->upstreamActuator()->position()->positionControl());
			upstreamStatusEditor_->setControl(filterFarm_->upstreamActuator()->position()->statusControl());
		}

		if (filterFarm_->downstreamActuator() && filterFarm_->downstreamActuator()->position()) {
			downstreamPositionEditor_->setControl(filterFarm_->downstreamActuator()->position()->positionControl());
			downstreamStatusEditor_->setControl(filterFarm_->downstreamActuator()->position()->statusControl());
		}
	}
}

void BioXASCarbonFilterFarmView::setFilterFarm(BioXASCarbonFilterFarm *newFilterFarm)
{
	if (filterFarm_ != newFilterFarm) {
		filterFarm_ = newFilterFarm;
		refresh();

		emit filterFarmChanged(filterFarm_);
	}
}
