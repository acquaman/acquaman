#include "BioXASBeamStatusView.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASFrontEndBeamStatusControl *frontEndBeamStatus, QWidget *parent) :
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

	setFrontEndBeamStatusControl(frontEndBeamStatus);
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
	frontEndBeamStatusView_->refresh();
}

void BioXASBeamStatusView::setFrontEndBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl)
{
	frontEndBeamStatusView_->setBeamStatusControl(newControl);
}
