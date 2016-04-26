#include "BioXASUtilitiesView.h"
#include "beamline/AMBeamlineControlGroup.h"
#include "beamline/BioXAS/BioXASUtilities.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASUtilitiesGroup.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASUtilitiesView::BioXASUtilitiesView(BioXASUtilities *utilities, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	utilities_ = 0;

	// Create UI elements.

	statusEditor_ = new BioXASControlEditor(0);
	statusEditor_->setTitle("Status");

	shuttersEditor_ = new BioXASControlEditor(0);
	shuttersEditor_->setTitle("Shutters");
	shuttersEditor_->setMinimumWidth(150);

	// Create and set layouts.

	QVBoxLayout *controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(shuttersEditor_);

	QGroupBox *controlsBox = new QGroupBox();
	controlsBox->setFlat(true);
	controlsBox->setLayout(controlsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(statusEditor_);
	layout->addWidget(controlsBox);

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
	updateShuttersEditor();
}

void BioXASUtilitiesView::setUtilities(BioXASUtilities *newUtilities)
{
	if (utilities_ != newUtilities) {

		if (utilities_)
			disconnect( utilities_, 0, this, 0 );

		utilities_ = newUtilities;

		if (utilities_)
			connect( utilities_, SIGNAL(connected(bool)), this, SLOT(refresh()) );

		refresh();

		emit utilitiesChanged(utilities_);
	}
}

void BioXASUtilitiesView::updateStatusEditor()
{
	statusEditor_->setControl(utilities_);
}

void BioXASUtilitiesView::updateShuttersEditor()
{
	AMControl *shutters = 0;

	if (utilities_)
		shutters = utilities_->shutters();

	shuttersEditor_->setControl(shutters);
}

