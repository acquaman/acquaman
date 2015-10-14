#include "BioXASBeamStatusView.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	QLabel *frontEndBeamStatusPrompt = new QLabel("BioXAS: ");
	frontEndBeamStatusView_ = new BioXASFrontEndBeamStatusView(0);

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(frontEndBeamStatusPrompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(frontEndBeamStatusView_, 0, 1);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
	refresh();
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::clear()
{
	frontEndBeamStatusView_->clear();
}

void BioXASBeamStatusView::update()
{
	frontEndBeamStatusView_->update();
}

void BioXASBeamStatusView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

//	if (beamStatus_) {
//		frontEndBeamStatusView_->setBeamStatusControl(beamStatus_->frontEndBeamStatusControl());
//	}

	// Update the view.

	update();
}

void BioXASBeamStatusView::setBeamStatus(BioXASBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		beamStatus_ = newStatus;

		refresh();

		emit beamStatusChanged(beamStatus_);

	}
}
