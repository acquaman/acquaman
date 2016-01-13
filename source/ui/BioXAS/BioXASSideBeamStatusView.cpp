#include "BioXASSideBeamStatusView.h"
#include "beamline/BioXAS/BioXASSideBeamStatus.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASSideBeamStatusBar.h"

BioXASSideBeamStatusView::BioXASSideBeamStatusView(BioXASSideBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new BioXASControlEditor(0);
	beamStatusEditor_->setTitle("Beam status");

	statusBar_ = new BioXASSideBeamStatusBar(0);

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

BioXASSideBeamStatusView::~BioXASSideBeamStatusView()
{

}

void BioXASSideBeamStatusView::refresh()
{
	updateBeamStatusEditor();
	updateBeamStatusBar();
}

void BioXASSideBeamStatusView::setBeamStatus(BioXASSideBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {
		beamStatus_ = newStatus;

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSideBeamStatusView::updateBeamStatusEditor()
{
	beamStatusEditor_->setControl(beamStatus_);
}

void BioXASSideBeamStatusView::updateBeamStatusBar()
{
	statusBar_->setBeamStatus(beamStatus_);
}
