#include "BioXASM2MirrorView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASM2MirrorView::BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	screenEditor_ = 0;

	// Create and set layout.

	layout_ = new QVBoxLayout();
	setLayout(layout_);

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

			// Clear UI elements.

			layout_->removeWidget(screenEditor_);

			screenEditor_->deleteLater();

			screenEditor_ = 0;
		}

		mirror_ = newMirror;

		if (mirror_) {

			// Create UI elements.

			screenEditor_ = new AMExtendedControlEditor(mirror_->screenControl());
			layout_->addWidget(screenEditor_);
		}
	}
}
