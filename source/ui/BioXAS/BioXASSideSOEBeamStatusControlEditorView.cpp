#include "BioXASSideSOEBeamStatusControlEditorView.h"

BioXASSideSOEBeamStatusControlEditorView::BioXASSideSOEBeamStatusControlEditorView(BioXASSideSOEBeamStatusControl *control, QWidget *parent) :
    BioXASSideSOEBeamStatusControlView(parent)
{
	// Create UI elements.

	editor_ = new AMExtendedControlEditor(0);
	editor_->setNoUnitsBox(true);

	ledView_ = new AMControlValueLEDView(0);
	ledView_->setGreenValue(CLSBiStateControl::Open);
	ledView_->setToolTip("Endstation shutter");

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

BioXASSideSOEBeamStatusControlEditorView::~BioXASSideSOEBeamStatusControlEditorView()
{

}

void BioXASSideSOEBeamStatusControlEditorView::clear()
{
	editor_->setControl(0);
	ledView_->setControl(0);
}

void BioXASSideSOEBeamStatusControlEditorView::update()
{
	editor_->setControl(control_);

	if (control_)
		ledView_->setControl(control_->endstationShutter());
}

void BioXASSideSOEBeamStatusControlEditorView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}
