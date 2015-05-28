#include "BioXASM2MirrorView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASM2MirrorView::BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	mirrorEditor_ = new BioXASMirrorView(0);

	screenEditor_ = new AMExtendedControlEditor(0);
	screenEditor_->setNoUnitsBox(true);
	screenEditor_->setTitle("Fluorescent screen");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(mirrorEditor_);
	layout->addWidget(screenEditor_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASM2MirrorView::~BioXASM2MirrorView()
{

}

void BioXASM2MirrorView::setMirror(BioXASM2Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_) {
			mirrorEditor_->setMirror(mirror_);
			screenEditor_->setControl(0);
		}

		mirror_ = newMirror;

		if (mirror_) {
			mirrorEditor_->setMirror(mirror_);
			screenEditor_->setControl(mirror_->screenMotorControl());
		}
	}
}
