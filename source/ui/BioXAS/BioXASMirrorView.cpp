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

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(upstreamInboardEditor_);
	layout->addWidget(upstreamOutboardEditor_);
	layout->addWidget(downstreamEditor_);

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
		}

		mirror_ = newMirror;

		if (mirror_) {
			upstreamInboardEditor_->setControl(mirror_->upstreamInboardControl());
			upstreamOutboardEditor_->setControl(mirror_->upstreamOutboardControl());
			downstreamEditor_->setControl(mirror_->downstreamControl());
		}

		emit mirrorChanged(mirror_);
	}
}
