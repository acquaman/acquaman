#include "BioXASXIAFiltersView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include <QDebug>

BioXASXIAFiltersView::BioXASXIAFiltersView(BioXASXIAFilters *filters, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filters_ = 0;

	editor1_ = 0;
	editor2_ = 0;
	editor3_ = 0;
	editor4_ = 0;

	// Create and set layout.

	layout_ = new QVBoxLayout();
	setLayout(layout_);

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

			// Clear UI elements.

			layout_->removeWidget(editor1_);
			layout_->removeWidget(editor2_);
			layout_->removeWidget(editor3_);
			layout_->removeWidget(editor4_);

			editor1_->deleteLater();
			editor2_->deleteLater();
			editor3_->deleteLater();
			editor4_->deleteLater();

			editor1_ = 0;
			editor2_ = 0;
			editor3_ = 0;
			editor4_ = 0;
		}

		filters_ = newFilters;

		if (filters_) {

			// Create new UI elements.

			editor1_ = new AMExtendedControlEditor(filters_->filter1());
			editor2_ = new AMExtendedControlEditor(filters_->filter2());
			editor3_ = new AMExtendedControlEditor(filters_->filter3());
			editor4_ = new AMExtendedControlEditor(filters_->filter4());

			layout_->addWidget(editor1_);
			layout_->addWidget(editor2_);
			layout_->addWidget(editor3_);
			layout_->addWidget(editor4_);
		}

		emit filtersChanged(filters_);
	}
}
