#include "BioXASFrontEndBeamStatusView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASMasterValves.h"

BioXASFrontEndBeamStatusView::BioXASFrontEndBeamStatusView(BioXASFrontEndBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setTitle("Front-end beam status");
	beamStatusEditor_->setNoUnitsBox(true);

	shuttersEditor_ = new AMExtendedControlEditor(0);
	shuttersEditor_->setTitle("Shutters");
	shuttersEditor_->setNoUnitsBox(true);

	valvesEditor_ = new AMExtendedControlEditor(0);
	valvesEditor_->setTitle("Valves");
	valvesEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *subControlsLayout = new QHBoxLayout();
	subControlsLayout->addWidget(shuttersEditor_);
	subControlsLayout->addWidget(valvesEditor_);

	QGroupBox *subControlsBox = new QGroupBox();
	subControlsBox->setFlat(true);
	subControlsBox->setLayout(subControlsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(subControlsBox);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASFrontEndBeamStatusView::~BioXASFrontEndBeamStatusView()
{

}

void BioXASFrontEndBeamStatusView::refresh()
{
	// Clear the view.

	beamStatusEditor_->setControl(0);
	shuttersEditor_->setControl(0);
	valvesEditor_->setControl(0);

	// Update view elements.

	beamStatusEditor_->setControl(beamStatus_);

	updateShuttersEditor();
	updateValvesEditor();
}

void BioXASFrontEndBeamStatusView::setBeamStatus(BioXASFrontEndBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(shuttersChanged(BioXASFrontEndShutters*)), this, SLOT(updateShuttersEditor()) );
			connect( beamStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(updateValvesEditor()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASFrontEndBeamStatusView::updateShuttersEditor()
{
	AMControl *shuttersControl = 0;

	if (beamStatus_)
		shuttersControl = beamStatus_->shutters();

	shuttersEditor_->setControl(shuttersControl);
}

void BioXASFrontEndBeamStatusView::updateValvesEditor()
{
	AMControl *valvesControl = 0;

	if (beamStatus_)
		valvesControl = beamStatus_->valves();

	valvesEditor_->setControl(valvesControl);
}
