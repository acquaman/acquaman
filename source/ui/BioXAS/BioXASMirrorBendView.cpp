#include "BioXASMirrorBendView.h"

BioXASMirrorBendView::BioXASMirrorBendView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	bendEditor_ = new AMExtendedControlEditor(0);
	bendEditor_->setControlFormat('f', 3);
	bendEditor_->setTitle("Bend radius");

	upstreamEditor_ = new AMExtendedControlEditor(0);
	upstreamEditor_->setTitle("Upstream bender");

	downstreamEditor_ = new AMExtendedControlEditor(0);
	downstreamEditor_->setTitle("Downstream bender");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(bendEditor_);
	layout->addWidget(upstreamEditor_);
	layout->addWidget(downstreamEditor_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASMirrorBendView::~BioXASMirrorBendView()
{

}

void BioXASMirrorBendView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_) {
			bendEditor_->setControl(0);
			upstreamEditor_->setControl(0);
			downstreamEditor_->setControl(0);
		}

		mirror_ = newMirror;

		if (mirror_) {
			bendEditor_->setControl(mirror_->bendControl());
			upstreamEditor_->setControl(mirror_->benderUpstreamMotorControl());
			downstreamEditor_->setControl(mirror_->benderDownstreamMotorControl());
		}

		emit mirrorChanged(mirror_);
	}
}
