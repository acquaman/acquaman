#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

	// Create UI elements.

	QLabel *upstreamPrompt = new QLabel("Upstream actuator:");
	upstreamWindowEditor_ = new AMExtendedControlEditor(0);
	upstreamThicknessEditor_ = new AMExtendedControlEditor(0);

	QLabel *downstreamPrompt = new QLabel("Downstream actuator:");
	downstreamWindowEditor_ = new AMExtendedControlEditor(0);
	downstreamThicknessEditor_ = new AMExtendedControlEditor(0);

	totalThicknessEditor_ = new AMExtendedControlEditor(0);

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(upstreamPrompt, )

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
