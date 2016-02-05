#include "BioXASFilterFlipperConfigurationView.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilters.h"
#include "ui/BioXAS/BioXASFilterFlipperFilterView.h"

BioXASFilterFlipperFiltersConfigurationView::BioXASFilterFlipperFiltersConfigurationView(BioXASFilterFlipperFilters *filters, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filters_ = 0;

	// Create and set layouts.

	layout_ = new QFormLayout();

	setLayout(layout_);

	// Current settings.

	setFilters(filters);
	refresh();
}

BioXASFilterFlipperFiltersConfigurationView::~BioXASFilterFlipperFiltersConfigurationView()
{

}

void BioXASFilterFlipperFiltersConfigurationView::refresh()
{
	// Clear the view.

	clear();

	// Create new filter views for each flipper slide filter.

	if (filters_) {

		QList<int> slideIndices = filters_->indices();

		// We skip index = 0, as it represents the 'None' case--not an actual
		// filter to be configured.

		foreach (int index, slideIndices) {

			if (index >= 1) {
				QHBoxLayout *filterViewLayout = new QHBoxLayout();
				filterViewLayout->addWidget(new QLabel(QString("%1 :").arg(index)));
				filterViewLayout->addWidget(new BioXASFilterFlipperFilterView(filters_->filterAt(index)));

				QWidget *filterView = new QWidget();
				filterView->setLayout(filterViewLayout);

				filterViews_.append(filterView);

				layout_->addRow(filterView);
			}
		}
	}
}

void BioXASFilterFlipperFiltersConfigurationView::setFilters(BioXASFilterFlipperFilters *newFilters)
{
	if (filters_ != newFilters) {
		filters_ = newFilters;

		refresh();

		emit filtersChanged(filters_);
	}
}

void BioXASFilterFlipperFiltersConfigurationView::clear()
{
	// Remove all existing filter views from the layout,
	// and delete them.

	foreach (QWidget *filterView, filterViews_) {
		layout_->removeWidget(filterView);
		filterViews_.removeOne(filterView);

		filterView->deleteLater();
	}
}
