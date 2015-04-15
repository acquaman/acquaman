#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

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
			layout()->deleteLater();
			setLayout(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Create new UI elements.

			AMExtendedControlEditor *filterFarmEditor = new AMExtendedControlEditor(filterFarm_);

			// Create and set new layout, takes ownership of the UI elements.

			QVBoxLayout *layout = new QVBoxLayout();
			layout->addWidget(filterFarmEditor);

			setLayout(layout);
		}

		emit filterFarmChanged(filterFarm_);
	}
}
