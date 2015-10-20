#include "BioXASFrontEndBeamStatusView.h"

BioXASFrontEndBeamStatusControlView::BioXASFrontEndBeamStatusControlView(BioXASFrontEndBeamStatusControl *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	editor_ = new AMExtendedControlEditor(0);
	editor_->setNoUnitsBox(true);

	ledView_ = new BioXASFrontEndBeamStatusControlLEDView(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(editor_);
	layout->addWidget(ledView_);

	setLayout(layout);

	// Current settings.

	setControl(beamStatus);
	refresh();
}

BioXASFrontEndBeamStatusControlView::~BioXASFrontEndBeamStatusControlView()
{

}

void BioXASFrontEndBeamStatusControlView::clear()
{
	editor_->setControl(0);
	ledView_->setControl(0);
}

void BioXASFrontEndBeamStatusControlView::update()
{
	ledView_->update();
}

void BioXASFrontEndBeamStatusControlView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

	if (control_) {
		editor_->setControl(control_);
		ledView_->setControl(control_);
	}

	// Update the view.

	update();
}

void BioXASFrontEndBeamStatusControlView::setControl(BioXASFrontEndBeamStatusControl *newControl)
{
	if (control_ != newControl) {
		control_ = newControl;
		refresh();

		emit controlChanged(control_);
	}
}
