#include "BioXASFrontEndShuttersView.h"
#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASFrontEndShuttersView::BioXASFrontEndShuttersView(BioXASFrontEndShutters *shutters, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	shutters_ = 0;

	// Create UI elements.

	shuttersEditor_ = new AMExtendedControlEditor(0);
	shuttersEditor_->setTitle("Shutters");
	shuttersEditor_->setNoUnitsBox(true);

	psh1_ = new AMExtendedControlEditor(0);
	psh1_->setNoUnitsBox(true);

	psh2_ = new AMExtendedControlEditor(0);
	psh2_->setNoUnitsBox(true);

	ssh_ = new AMExtendedControlEditor(0);
	ssh_->setNoUnitsBox(true);

	// Create and set layouts.

	QVBoxLayout *shuttersLayout = new QVBoxLayout();
	shuttersLayout->addWidget(psh1_);
	shuttersLayout->addWidget(psh2_);
	shuttersLayout->addWidget(ssh_);

	QGroupBox *shuttersBox = new QGroupBox();
	shuttersBox->setFlat(true);
	shuttersBox->setLayout(shuttersLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(shuttersEditor_);
	layout->addWidget(shuttersBox);

	setLayout(layout);

	// Current settings.

	setShutters(shutters);
	refresh();
}

BioXASFrontEndShuttersView::~BioXASFrontEndShuttersView()
{

}

void BioXASFrontEndShuttersView::clear()
{
	shuttersEditor_->setControl(0);
	psh1_->setControl(0);
	psh2_->setControl(0);
	ssh_->setControl(0);
}

void BioXASFrontEndShuttersView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	shuttersEditor_->setControl(shutters_);

	if (shutters_) {
		psh1_->setControl(shutters_->upstreamPhotonShutter());
		psh2_->setControl(shutters_->downstreamPhotonShutter());
		ssh_->setControl(shutters_->safetyShutter());
	}
}

void BioXASFrontEndShuttersView::setShutters(BioXASFrontEndShutters *newShutters)
{
	if (shutters_ != newShutters) {
		shutters_ = newShutters;
		refresh();

		emit shuttersChanged(shutters_);
	}
}
