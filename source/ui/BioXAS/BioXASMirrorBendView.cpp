#include "BioXASMirrorBendView.h"

BioXASMirrorBendView::BioXASMirrorBendView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	bendEditor_ = new AMExtendedControlEditor(0);
	bendEditor_->setControlFormat('f', 2);
	bendEditor_->setTitle("Bend radius");

	upstreamEditor_ = new AMExtendedControlEditor(0);
	upstreamEditor_->setControlFormat('f', 2);
	upstreamEditor_->setTitle("Upstream bender");

	downstreamEditor_ = new AMExtendedControlEditor(0);
	downstreamEditor_->setControlFormat('f', 2);
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

void BioXASMirrorBendView::refresh()
{
	// Clear the view.

	bendEditor_->setControl(0);
	upstreamEditor_->setControl(0);
	downstreamEditor_->setControl(0);

	// Update view element.

	if (mirror_) {
		bendEditor_->setControl(mirror_->bend());
		upstreamEditor_->setControl(mirror_->upstreamBenderMotor());
		downstreamEditor_->setControl(mirror_->downstreamBenderMotor());
	}
}

void BioXASMirrorBendView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {
		mirror_ = newMirror;
		refresh();

		emit mirrorChanged(mirror_);
	}
}
