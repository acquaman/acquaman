#include "BioXASFilterFlipperView.h"

BioXASFilterFlipperView::BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create UI elements.

	configurationView_ = new BioXASFilterFlipperConfigurationView(0, this);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(configurationView_);

	setLayout(layout);

	// Current settings.

	setFilterFlipper(filterFlipper);
}

BioXASFilterFlipperView::~BioXASFilterFlipperView()
{

}

void BioXASFilterFlipperView::clear()
{
	configurationView_->clear();
}

void BioXASFilterFlipperView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateFilterConfigurationView();
}

void BioXASFilterFlipperView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {

		if (filterFlipper_)
			disconnect( filterFlipper_, 0, this, 0 );

		filterFlipper_ = newFlipper;


		if (filterFlipper_) {

		}

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}

void BioXASFilterFlipperView::updateFilterConfigurationView()
{
	configurationView_->setFilterFlipper(filterFlipper_);
}
