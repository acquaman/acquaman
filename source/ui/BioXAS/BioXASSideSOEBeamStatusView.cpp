#include "BioXASSideSOEBeamStatusView.h"
#include "beamline/BioXAS/BioXASSideSOEBeamStatus.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASSideSOEBeamStatusView::BioXASSideSOEBeamStatusView(BioXASSideSOEBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new BioXASControlEditor(0);
	beamStatusEditor_->setTitle("SOE beam status");

	shutterEditor_ = new BioXASControlEditor(0);
	shutterEditor_->setTitle("Shutter");

	// Create and set layouts.

	QVBoxLayout *subControlsLayout = new QVBoxLayout();
	subControlsLayout->addWidget(shutterEditor_);

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

	refresh();
}

BioXASSideSOEBeamStatusView::~BioXASSideSOEBeamStatusView()
{

}

void BioXASSideSOEBeamStatusView::refresh()
{
	// Clear the view.

	beamStatusEditor_->setControl(0);
	shutterEditor_->setControl(0);

	// Update view elements.

	beamStatusEditor_->setControl(beamStatus_);

	updateShutterEditor();
}

void BioXASSideSOEBeamStatusView::setBeamStatus(BioXASSideSOEBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(endstationShutterChanged(CLSBiStateControl*)), this, SLOT(updateShutterEditor()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSideSOEBeamStatusView::updateShutterEditor()
{
	AMControl *shutterControl = 0;

	if (beamStatus_)
		shutterControl = beamStatus_->endstationShutter();

	shutterEditor_->setControl(shutterControl);
}
