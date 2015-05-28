#include "BioXASMirrorView.h"

BioXASMirrorView::BioXASMirrorView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	upstreamInboardEditor_ = new AMExtendedControlEditor(0);
	upstreamInboardEditor_->setTitle("Upstream inboard -- vertical");

	upstreamOutboardEditor_ = new AMExtendedControlEditor(0);
	upstreamOutboardEditor_->setTitle("Upstream outboard -- vertical");

	downstreamEditor_ = new AMExtendedControlEditor(0);
	downstreamEditor_->setTitle("Downstream -- vertical");

	pitchEditor_ = new AMExtendedControlEditor(0);
	pitchEditor_->setTitle("Pitch");

	rollEditor_ = new AMExtendedControlEditor(0);
	rollEditor_->setTitle("Roll");

	yawEditor_ = new AMExtendedControlEditor(0);
	yawEditor_->setTitle("Yaw");

	heightEditor_ = new AMExtendedControlEditor(0);
	heightEditor_->setTitle("Height");

	lateralEditor_ = new AMExtendedControlEditor(0);
	lateralEditor_->setTitle("Lateral");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(upstreamInboardEditor_);
	layout->addWidget(upstreamOutboardEditor_);
	layout->addWidget(downstreamEditor_);
	layout->addWidget(pitchEditor_);
	layout->addWidget(rollEditor_);
	layout->addWidget(yawEditor_);
	layout->addWidget(heightEditor_);
	layout->addWidget(lateralEditor_);

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
			upstreamInboardEditor_->setControl(0);
			upstreamOutboardEditor_->setControl(0);
			downstreamEditor_->setControl(0);
			pitchEditor_->setControl(0);
			rollEditor_->setControl(0);
			yawEditor_->setControl(0);
			heightEditor_->setControl(0);
			lateralEditor_->setControl(0);
		}

		mirror_ = newMirror;

		if (mirror_) {
			upstreamInboardEditor_->setControl(mirror_->upstreamInboardMotorControl());
			upstreamOutboardEditor_->setControl(mirror_->upstreamOutboardMotorControl());
			downstreamEditor_->setControl(mirror_->downstreamMotorControl());

			pitchEditor_->setControl(mirror_->pitchControl());
			rollEditor_->setControl(mirror_->rollControl());
			yawEditor_->setControl(mirror_->yawControl());
			heightEditor_->setControl(mirror_->heightControl());
			lateralEditor_->setControl(mirror_->lateralControl());
		}

		emit mirrorChanged(mirror_);
	}
}
