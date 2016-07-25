#include "BioXASM2MirrorView.h"

BioXASM2MirrorView::BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	screenEditor_ = new CLSControlEditor(0);
	screenEditor_->setTitle("Fluorescent screen");

	mirrorEditor_ = new BioXASMirrorView(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(screenEditor_);
	layout->addWidget(mirrorEditor_);

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

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_)
			connect( mirror_, SIGNAL(screenChanged(AMControl*)), this, SLOT(updateScreenEditor()) );

		updateMirrorView();

		emit mirrorChanged(mirror_);
	}
}

void BioXASM2MirrorView::updateMirrorView()
{
	updateScreenEditor();
	updateMirrorEditor();
}

void BioXASM2MirrorView::updateScreenEditor()
{
	screenEditor_->setControl(mirror_ ? mirror_->screen() : 0);
}

void BioXASM2MirrorView::updateMirrorEditor()
{
	mirrorEditor_->setMirror(mirror_);
}
