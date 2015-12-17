#include "BioXASM2MirrorView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASM2MirrorView::BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	stopButton_ = new AMControlStopButton(0);

	screenEditor_ = new AMExtendedControlEditor(0);
	screenEditor_->setNoUnitsBox(true);
	screenEditor_->setTitle("Fluorescent screen");

	mirrorView_ = new BioXASMirrorView(0);

	// Create and set layouts.

	QHBoxLayout *stopButtonLayout = new QHBoxLayout();
	stopButtonLayout->addStretch();
	stopButtonLayout->addWidget(stopButton_);
	stopButtonLayout->addStretch();

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(stopButtonLayout);
	layout->addWidget(screenEditor_);
	layout->addWidget(mirrorView_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);

	refresh();
}

BioXASM2MirrorView::~BioXASM2MirrorView()
{

}

void BioXASM2MirrorView::refresh()
{
	// Clear the view.

	stopButton_->setControl(0);
	screenEditor_->setControl(0);
	mirrorView_->setMirror(0);

	// Update view elements.

	if (mirror_) {
		stopButton_->setControl(mirror_);
		updateScreenEditor();
		mirrorView_->setMirror(mirror_);
	}
}

void BioXASM2MirrorView::setMirror(BioXASM2Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_)
			connect( mirror_, SIGNAL(screenChanged(AMControl*)), this, SLOT(updateScreenEditor()) );

		refresh();

		emit mirrorChanged(mirror_);
	}
}

void BioXASM2MirrorView::updateScreenEditor()
{
	AMControl *screenControl = 0;

	if (mirror_)
		screenControl = mirror_->screen();

	screenEditor_->setControl(screenControl);
}
