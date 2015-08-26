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
	layout->addWidget(upstreamPrompt, 0, 0);
	layout->addWidget(upstreamWindowEditor_, 0, 1);
	layout->addWidget(upstreamThicknessEditor_, 0, 2);
	layout->addWidget(downstreamPrompt, 1, 0);
	layout->addWidget(downstreamWindowEditor_, 1, 1);
	layout->addWidget(downstreamThicknessEditor_, 1, 2);
	layout->addWidget(totalThicknessEditor_, 2, 2);

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

			upstreamWindowEditor_->setControl(0);
			upstreamThicknessEditor_->setControl(0);
			downstreamWindowEditor_->setControl(0);
			downstreamThicknessEditor_->setControl(0);
			totalThicknessEditor_->setControl(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Update UI elements.

			upstreamWindowEditor_->setControl(filterFarm_->upstreamActuatorWindowControl());
			upstreamThicknessEditor_->setControl(filterFarm_->upstreamActuatorFilterThicknessControl());
			downstreamWindowEditor_->setControl(filterFarm_->downstreamActuatorWindowControl());
			downstreamThicknessEditor_->setControl(filterFarm_->downstreamActuatorFilterThicknessControl());
			totalThicknessEditor_->setControl(filterFarm_->filterThicknessControl());
		}

		emit filterFarmChanged(filterFarm_);
	}
}
