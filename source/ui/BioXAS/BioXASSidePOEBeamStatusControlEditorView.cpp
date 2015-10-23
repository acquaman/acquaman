#include "BioXASSidePOEBeamStatusControlEditorView.h"

BioXASSidePOEBeamStatusControlEditorView::BioXASSidePOEBeamStatusControlEditorView(BioXASSidePOEBeamStatusControl *control, QWidget *parent) :
    BioXASSidePOEBeamStatusControlView(parent)
{
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

BioXASSidePOEBeamStatusControlEditorView::~BioXASSidePOEBeamStatusControlEditorView()
{

}

void BioXASSidePOEBeamStatusControlEditorView::clear()
{
	editor_->setControl(0);
	ledView_->setControl(0);
}

void BioXASSidePOEBeamStatusControlEditorView::update()
{
	ledView_->update();
}

void BioXASSidePOEBeamStatusControlEditorView::refresh()
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
