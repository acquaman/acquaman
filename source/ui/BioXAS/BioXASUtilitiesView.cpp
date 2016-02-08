#include "BioXASUtilitiesView.h"
#include "beamline/BioXAS/BioXASUtilities.h"
#include "beamline/BioXAS/BioXASUtilitiesState.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASUtilitiesView::BioXASUtilitiesView(BioXASUtilities *utilities, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	utilities_ = 0;

	// Create UI elements.

	statusEditor_ = new BioXASControlEditor(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(statusEditor_);

	setLayout(layout);

	// Current settings.

	setUtilities(utilities);
	refresh();
}

BioXASUtilitiesView::~BioXASUtilitiesView()
{

}

void BioXASUtilitiesView::refresh()
{
	updateStatusEditor();
}

void BioXASUtilitiesView::setUtilities(BioXASUtilities *newUtilities)
{
	if (utilities_ != newUtilities) {
		utilities_ = newUtilities;
		refresh();

		emit utilitiesChanged(utilities_);
	}
}

void BioXASUtilitiesView::updateStatusEditor()
{
	AMControl *status = 0;

	if (utilities_)
		status = utilities_->state();

	statusEditor_->setControl(status);
}

