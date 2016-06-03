#include "BioXASFilterFlipperView.h"
#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "ui/BioXAS/BioXASFilterFlipperConfigurationView.h"
#include "ui/CLS/CLSControlEditor.h"

BioXASFilterFlipperView::BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create UI elements.

	filtersEditor_ = new CLSControlEditor(0);
	filtersEditor_->setTitle("Current filter");
	filtersEditor_->setMinimumWidth(200);

	changeStatusEditor_ = new CLSControlEditor(0);
	changeStatusEditor_->setTitle("Change status");

	configurationView_ = new BioXASFilterFlipperFiltersConfigurationView(0, this);

	// Create and set layouts.

	QHBoxLayout *filtersLayout = new QHBoxLayout();
	filtersLayout->setMargin(0);
	filtersLayout->addWidget(filtersEditor_);
	filtersLayout->addWidget(changeStatusEditor_);

	QVBoxLayout *configurationBoxLayout = new QVBoxLayout();
	configurationBoxLayout->setMargin(0);
	configurationBoxLayout->addWidget(configurationView_);

	QGroupBox *configurationBox = new QGroupBox();
	configurationBox->setTitle("Filter configuration");
	configurationBox->setLayout(configurationBoxLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(filtersLayout);
	layout->addWidget(configurationBox);

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

	updateFiltersEditor();
	updateChangeStatusEditor();
	updateFilterConfigurationView();
}

void BioXASFilterFlipperView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {

		if (filterFlipper_ && filterFlipper_->filters())
			disconnect( filterFlipper_->filters(), 0, this, 0 );

		filterFlipper_ = newFlipper;


		if (filterFlipper_ && filterFlipper_->filters())
//			connect( filterFlipper_->filters(), SIGNAL(filtersChanged()), this, SLOT(refreshFiltersEditor()) );

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}

void BioXASFilterFlipperView::clear()
{
	filtersEditor_->setControl(0);
	changeStatusEditor_->setControl(0);
	configurationView_->setFilters(0);
}

void BioXASFilterFlipperView::updateFiltersEditor()
{
	AMControl *filtersControl = 0;

	if (filterFlipper_)
		filtersControl = filterFlipper_->filters();

	filtersEditor_->setControl(filtersControl);
}

void BioXASFilterFlipperView::updateChangeStatusEditor()
{
	AMControl *statusControl = 0;

	if (filterFlipper_)
		statusControl = filterFlipper_->slideChangeStatus();

	changeStatusEditor_->setControl(statusControl);
}

void BioXASFilterFlipperView::updateFilterConfigurationView()
{
	BioXASFilterFlipperFilters *filterControl = 0;

	if (filterFlipper_)
		filterControl = filterFlipper_->filters();

	configurationView_->setFilters(filterControl);
}

void BioXASFilterFlipperView::refreshFiltersEditor()
{
	filtersEditor_->setControl(0);
	updateFiltersEditor();
}
