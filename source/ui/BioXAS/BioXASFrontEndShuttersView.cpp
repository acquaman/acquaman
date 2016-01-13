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

void BioXASFrontEndShuttersView::refresh()
{
	// Clear the view.

	shuttersEditor_->setControl(0);
	psh1_->setControl(0);
	psh2_->setControl(0);
	ssh_->setControl(0);

	// Update the view.

	shuttersEditor_->setControl(shutters_);

	updateUpstreamPhotonShutterEditor();
	updateDownstreamPhotonShutterEditor();
	updateSafetyShutterEditor();
}

void BioXASFrontEndShuttersView::setShutters(BioXASFrontEndShutters *newShutters)
{
	if (shutters_ != newShutters) {

		if (shutters_)
			disconnect( shutters_, 0, this, 0 );

		shutters_ = newShutters;

		if (shutters_) {
			connect( shutters_, SIGNAL(upstreamPhotonShutterChanged(AMControl*)), this, SLOT(updateUpstreamPhotonShutterEditor()) );
			connect( shutters_, SIGNAL(downstreamPhotonShutterChanged(AMControl*)), this, SLOT(updateDownstreamPhotonShutterEditor()) );
			connect( shutters_, SIGNAL(safetyShutterChanged(AMControl*)), this, SLOT(updateSafetyShutterEditor()) );
		}

		refresh();

		emit shuttersChanged(shutters_);
	}
}

void BioXASFrontEndShuttersView::updateUpstreamPhotonShutterEditor()
{
	AMControl *upstreamShutter = 0;

	if (shutters_)
		upstreamShutter = shutters_->upstreamPhotonShutter();

	psh1_->setControl(upstreamShutter);
}

void BioXASFrontEndShuttersView::updateDownstreamPhotonShutterEditor()
{
	AMControl *downstreamShutter = 0;

	if (shutters_)
		downstreamShutter = shutters_->downstreamPhotonShutter();

	psh2_->setControl(downstreamShutter);
}

void BioXASFrontEndShuttersView::updateSafetyShutterEditor()
{
	AMControl *safetyShutter = 0;

	if (shutters_)
		safetyShutter = shutters_->safetyShutter();

	ssh_->setControl(safetyShutter);
}
