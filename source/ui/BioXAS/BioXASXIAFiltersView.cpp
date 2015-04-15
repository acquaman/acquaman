#include "BioXASXIAFiltersView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASXIAFiltersView::BioXASXIAFiltersView(BioXASXIAFilters *filters, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filters_ = 0;

	// Current settings.

	setFilters(filters);
}

BioXASXIAFiltersView::~BioXASXIAFiltersView()
{

}

void BioXASXIAFiltersView::setFilters(BioXASXIAFilters *newFilters)
{
	if (filters_ != newFilters) {

		if (filters_) {
			layout()->deleteLater();
			setLayout(0);
		}

		filters_ = newFilters;

		if (filters_) {

			// Create new UI elements.

			AMExtendedControlEditor *filter1 = new AMExtendedControlEditor(filters_->filter1());
			AMExtendedControlEditor *filter2 = new AMExtendedControlEditor(filters_->filter2());
			AMExtendedControlEditor *filter3 = new AMExtendedControlEditor(filters_->filter3());
			AMExtendedControlEditor *filter4 = new AMExtendedControlEditor(filters_->filter4());

			// Create and set new layout, takes ownership of the UI elements.

			QVBoxLayout *layout = new QVBoxLayout();
			layout->addWidget(filter1);
			layout->addWidget(filter2);
			layout->addWidget(filter3);
			layout->addWidget(filter4);

			setLayout(layout);
		}

		emit filtersChanged(filters_);
	}
}
