#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;
	editor_ = 0;

	// Create and set main layout.

	layout_ = new QVBoxLayout();
	setLayout(layout_);

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

			layout_->removeWidget(editor_);
			editor_->deleteLater();
			editor_ = 0;
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Create new UI elements.

			editor_ = new AMExtendedControlEditor(filterFarm_);
			layout_->addWidget(editor_);
		}

		emit filterFarmChanged(filterFarm_);
	}
}
