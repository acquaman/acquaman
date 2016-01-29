#include "BioXASBeamStatusView.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASBeamStatusBar.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new BioXASControlEditor(0);
	beamStatusEditor_->setTitle("Beam status");

	statusBar_ = new BioXASBeamStatusBar(0);

	QVBoxLayout *statusBarLayout = new QVBoxLayout();
	statusBarLayout->addWidget(statusBar_);

	QGroupBox *statusBox = new QGroupBox();
	statusBox->setTitle("Components");
	statusBox->setLayout(statusBarLayout);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(statusBox);

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
	updateBeamStatusBar();
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

void BioXASBeamStatusView::updateBeamStatusBar()
{
	statusBar_->setBeamStatus(beamStatus_);
}
