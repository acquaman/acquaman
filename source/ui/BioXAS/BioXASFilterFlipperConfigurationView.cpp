#include "BioXASFilterFlipperConfigurationView.h"

BioXASFilterFlipperConfigurationView::BioXASFilterFlipperConfigurationView(BioXASFilterFlipper *filterFlipper, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFlipper_ = 0;

	// Create and set layouts.

	layout_ = new QFormLayout();

	setLayout(layout_);

	// Current settings.

	setFilterFlipper(filterFlipper);
	refresh();
}

BioXASFilterFlipperConfigurationView::~BioXASFilterFlipperConfigurationView()
{

}

void BioXASFilterFlipperConfigurationView::clear()
{
	// Remove all existing filter views from the layout,
	// and delete them.

	foreach (QWidget *filterView, filterViews_) {
		layout_->removeWidget(filterView);
		filterViews_.removeOne(filterView);

		filterView->deleteLater();
	}
}

void BioXASFilterFlipperConfigurationView::refresh()
{
	// Clear the view.

	clear();

	// Create new filter views for each flipper slide filter.

	if (filterFlipper_) {

		QList<int> slideIndices = filterFlipper_->slideIndices();

		foreach (int index, slideIndices) {

			QHBoxLayout *filterViewLayout = new QHBoxLayout();
			filterViewLayout->addWidget(new QLabel(QString("%1 :").arg(index)));
			filterViewLayout->addWidget(new BioXASFilterFlipperFilterView(filterFlipper_->slideFilter(index)));

			QWidget *filterView = new QWidget();
			filterView->setLayout(filterViewLayout);

			filterViews_.append(filterView);

			layout_->addWidget(filterView);
		}
	}
}

void BioXASFilterFlipperConfigurationView::setFilterFlipper(BioXASFilterFlipper *newFlipper)
{
	if (filterFlipper_ != newFlipper) {
		filterFlipper_ = newFlipper;

		refresh();

		emit filterFlipperChanged(filterFlipper_);
	}
}
