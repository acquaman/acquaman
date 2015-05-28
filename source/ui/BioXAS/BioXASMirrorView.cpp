#include "BioXASMirrorView.h"

BioXASMirrorView::BioXASMirrorView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	pitchEditor_ = new AMExtendedControlEditor(0);
	pitchEditor_->setControlFormat('f', 3);
	pitchEditor_->setTitle("Pitch");

	rollEditor_ = new AMExtendedControlEditor(0);
	rollEditor_->setControlFormat('f', 3);
	rollEditor_->setTitle("Roll");

	yawEditor_ = new AMExtendedControlEditor(0);
	yawEditor_->setControlFormat('f', 3);
	yawEditor_->setTitle("Yaw");

	heightEditor_ = new AMExtendedControlEditor(0);
	heightEditor_->setControlFormat('f', 3);
	heightEditor_->setTitle("Height");

	lateralEditor_ = new AMExtendedControlEditor(0);
	lateralEditor_->setControlFormat('f', 3);
	lateralEditor_->setTitle("Lateral");

	bendEditor_ = new AMExtendedControlEditor(0);
	bendEditor_->setControlFormat('f', 3);
	bendEditor_->setTitle("Bend radius");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(pitchEditor_);
	layout->addWidget(rollEditor_);
	layout->addWidget(yawEditor_);
	layout->addWidget(heightEditor_);
	layout->addWidget(lateralEditor_);
	layout->addWidget(bendEditor_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASMirrorView::~BioXASMirrorView()
{

}

void BioXASMirrorView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_) {
			pitchEditor_->setControl(0);
			rollEditor_->setControl(0);
			yawEditor_->setControl(0);
			heightEditor_->setControl(0);
			lateralEditor_->setControl(0);
			bendEditor_->setControl(0);
		}

		mirror_ = newMirror;

		if (mirror_) {
			pitchEditor_->setControl(mirror_->pitchControl());
			rollEditor_->setControl(mirror_->rollControl());
			yawEditor_->setControl(mirror_->yawControl());
			heightEditor_->setControl(mirror_->heightControl());
			lateralEditor_->setControl(mirror_->lateralControl());
			bendEditor_->setControl(mirror_->bendControl());
		}

		emit mirrorChanged(mirror_);
	}
}
