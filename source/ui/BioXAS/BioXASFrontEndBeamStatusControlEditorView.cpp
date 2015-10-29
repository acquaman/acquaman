#include "BioXASFrontEndBeamStatusControlEditorView.h"

BioXASFrontEndBeamStatusControlEditorView::BioXASFrontEndBeamStatusControlEditorView(BioXASFrontEndBeamStatusControl *control, QWidget *parent) :
	BioXASFrontEndBeamStatusControlView(parent)
{
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

	setControl(control);
	refresh();
}

BioXASFrontEndBeamStatusControlEditorView::~BioXASFrontEndBeamStatusControlEditorView()
{

}

void BioXASFrontEndBeamStatusControlEditorView::clear()
{
	editor_->setControl(0);
	ledView_->setControl(0);
}

void BioXASFrontEndBeamStatusControlEditorView::update()
{
	if (control_) {
		editor_->setControl(control_);
		ledView_->setControl(control_);

	} else {
		editor_->setControl(0);
		ledView_->setControl(0);
	}
}
