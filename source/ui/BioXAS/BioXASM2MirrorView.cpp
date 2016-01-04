#include "BioXASM2MirrorView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASM2MirrorView::BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	stopButton_ = new AMControlStopButton(0);

	mirrorEditor_ = new BioXASMirrorView(0);

	screenEditor_ = new AMExtendedControlEditor(0);
	screenEditor_->setNoUnitsBox(true);
	screenEditor_->setTitle("Fluorescent screen");

	bendView_ = new BioXASMirrorBendView(0);

	// Create and set layouts.

	QHBoxLayout *stopButtonLayout = new QHBoxLayout();
	stopButtonLayout->addStretch();
	stopButtonLayout->addWidget(stopButton_);
	stopButtonLayout->addStretch();

	QVBoxLayout *controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(mirrorEditor_);
	controlsLayout->addWidget(screenEditor_);

	QVBoxLayout *bendLayout = new QVBoxLayout();
	bendLayout->addWidget(bendView_);
	bendLayout->addStretch();

	QHBoxLayout *mirrorLayout = new QHBoxLayout();
	mirrorLayout->addLayout(controlsLayout);
	mirrorLayout->addLayout(bendLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(stopButtonLayout);
	layout->addLayout(mirrorLayout);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASM2MirrorView::~BioXASM2MirrorView()
{

}

void BioXASM2MirrorView::refresh()
{
	// Clear the view.

	stopButton_->setControl(0);
	mirrorEditor_->setMirror(0);
	screenEditor_->setControl(0);
	bendView_->setMirror(0);

	// Update view elements.

	if (mirror_) {
		stopButton_->setControl(mirror_);
		mirrorEditor_->setMirror(mirror_);
		updateScreenEditor();
		bendView_->setMirror(mirror_);
	}
}

void BioXASM2MirrorView::setMirror(BioXASM2Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(screenChanged(AMControl*)), this, SLOT(updateScreenEditor()) );
		}

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
