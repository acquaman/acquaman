#include "BioXASFilterFlipperView.h"
#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASFilterFlipperConfigurationView.h"

BioXASFilterFlipperView::BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create UI elements.

	slidesEditor_ = new AMExtendedControlEditor(0);
	slidesEditor_->setTitle("Current slide");
	slidesEditor_->setNoUnitsBox(true);

	filtersEditor_ = new AMExtendedControlEditor(0);
	filtersEditor_->setTitle("Current filter");
	filtersEditor_->setNoUnitsBox(true);

	configurationView_ = new BioXASFilterFlipperFiltersConfigurationView(0, this);

	// Create and set layouts.

	QHBoxLayout *slidesFiltersLayout = new QHBoxLayout();
	slidesFiltersLayout->addWidget(slidesEditor_);
	slidesFiltersLayout->addWidget(filtersEditor_);

	QVBoxLayout *configurationBoxLayout = new QVBoxLayout();
	configurationBoxLayout->addWidget(configurationView_);

	QGroupBox *configurationBox = new QGroupBox();
	configurationBox->setTitle("Filter configuration");
	configurationBox->setLayout(configurationBoxLayout);

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addLayout(slidesFiltersLayout);
	leftLayout->addStretch();

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(configurationBox);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(leftLayout);
	layout->addLayout(rightLayout);

	setLayout(layout);

	// Current settings.

	setFilterFlipper(filterFlipper);
}

BioXASFilterFlipperView::~BioXASFilterFlipperView()
{

}

void BioXASFilterFlipperView::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	updateSlidesEditor();
	updateFiltersEditor();
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

void BioXASFilterFlipperView::clear()
{
	slidesEditor_->setControl(0);
	filtersEditor_->setControl(0);
	configurationView_->setFilters(0);
}

void BioXASFilterFlipperView::updateSlidesEditor()
{
	if (filterFlipper_)
		slidesEditor_->setControl(filterFlipper_->slides());
}

void BioXASFilterFlipperView::updateFiltersEditor()
{
	if (filterFlipper_)
		filtersEditor_->setControl(filterFlipper_->filters());
}

void BioXASFilterFlipperView::updateFilterConfigurationView()
{
	if (filterFlipper_)
		configurationView_->setFilters(filterFlipper_->filters());
}
