#include "BioXASXIAFiltersView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include <QDebug>

BioXASXIAFiltersView::BioXASXIAFiltersView(BioXASXIAFilters *filters, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filters_ = 0;

	// Create UI elements.

	filtersControlEditor_ = new AMExtendedControlEditor(0);

	editor1_ = new AMExtendedControlEditor(0);
	editor2_ = new AMExtendedControlEditor(0);
	editor3_ = new AMExtendedControlEditor(0);
	editor4_ = new AMExtendedControlEditor(0);

	// Create and set layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(filtersControlEditor_);
	layout->addWidget(editor1_);
	layout->addWidget(editor2_);
	layout->addWidget(editor3_);
	layout->addWidget(editor4_);

	setLayout(layout);

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

			filtersControlEditor_->setControl(0);

			editor1_->setControl(0);
			editor2_->setControl(0);
			editor3_->setControl(0);
			editor4_->setControl(0);
		}

		filters_ = newFilters;

		if (filters_) {

			// Update UI elements.

			filtersControlEditor_->setControl(filters_->filtersControl());

			editor1_->setControl(filters_->filter1());
			editor2_->setControl(filters_->filter2());
			editor3_->setControl(filters_->filter3());
			editor4_->setControl(filters_->filter4());
		}

		emit filtersChanged(filters_);
	}
}
