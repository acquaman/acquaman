#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

	editor_ = new AMExtendedControlEditor(0);

	upstreamEditor_ = new AMExtendedControlEditor(0);
	downstreamEditor_ = new AMExtendedControlEditor(0);

	// Create and set layouts.

	QVBoxLayout *subControlLayout = new QVBoxLayout();
	subControlLayout->addWidget(upstreamEditor_);
	subControlLayout->addWidget(downstreamEditor_);

	QGroupBox *subControls = new QGroupBox("Sub-controls");
	subControls->setLayout(subControlLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(editor_);
	layout->addWidget(subControls);

	setLayout(layout);

	// Current settings.

	setFilterFarm(filterFarm);
}

BioXASCarbonFilterFarmView::~BioXASCarbonFilterFarmView()
{

}

void BioXASCarbonFilterFarmView::setFilterFarm(BioXASCarbonFilterFarmControl *newFilterFarm)
{
	if (filterFarm_ != newFilterFarm) {

		if (filterFarm_) {

			// Clear UI elements.

			editor_->setControl(0);

			upstreamEditor_->setControl(0);
			downstreamEditor_->setControl(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Update UI elements.

			editor_->setControl(filterFarm_);

			upstreamEditor_->setControl(filterFarm_->upstreamActuatorControl());
			downstreamEditor_->setControl(filterFarm_->downstreamActuatorControl());
		}

		emit filterFarmChanged(filterFarm_);
	}
}
