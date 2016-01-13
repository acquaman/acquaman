#include "BioXASBeamStatusView.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new BioXASControlEditor(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(beamStatusEditor_);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::refresh()
{
	updateBeamStatusEditor();
}

void BioXASBeamStatusView::setBeamStatus(BioXASBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {
		beamStatus_ = newStatus;

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASBeamStatusView::updateBeamStatusEditor()
{
	beamStatusEditor_->setControl(beamStatus_);
}
