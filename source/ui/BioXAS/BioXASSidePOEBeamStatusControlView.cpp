#include "BioXASSidePOEBeamStatusControlView.h"

BioXASSidePOEBeamStatusControlView::BioXASSidePOEBeamStatusControlView(BioXASSidePOEBeamStatusControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	editor_ = new AMExtendedControlEditor(0);
	editor_->setNoUnitsBox(true);

	ledView_ = new BioXASSidePOEBeamStatusControlLEDView(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(editor_);
	layout->addWidget(ledView_);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASSidePOEBeamStatusControlView::~BioXASSidePOEBeamStatusControlView()
{

}

void BioXASSidePOEBeamStatusControlView::clear()
{
	editor_->setControl(0);
	ledView_->setControl(0);
}

void BioXASSidePOEBeamStatusControlView::update()
{
	ledView_->update();
}

void BioXASSidePOEBeamStatusControlView::refresh()
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

void BioXASSidePOEBeamStatusControlView::setControl(BioXASSidePOEBeamStatusControl *newControl)
{
	if (control_ != newControl) {
		control_ = newControl;
		refresh();

		emit controlChanged(control_);
	}
}
