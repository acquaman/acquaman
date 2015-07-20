#include "BioXASM1MirrorView.h"

BioXASM1MirrorView::BioXASM1MirrorView(BioXASM1Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	mirrorEditor_ = new BioXASMirrorView(0);

	upperSlitEditor_ = new AMExtendedControlEditor(0);
	upperSlitEditor_->setTitle("Upper slit blade");

	bendView_ = new BioXASMirrorBendView(0);

	// Create and set layouts.

	QVBoxLayout *controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(mirrorEditor_);
	controlsLayout->addWidget(upperSlitEditor_);

	QVBoxLayout *bendLayout = new QVBoxLayout();
	bendLayout->addWidget(bendView_);
	bendLayout->addStretch();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(controlsLayout);
	layout->addLayout(bendLayout);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASM1MirrorView::~BioXASM1MirrorView()
{

}

void BioXASM1MirrorView::setMirror(BioXASM1Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_) {
			mirrorEditor_->setMirror(0);
			upperSlitEditor_->setControl(0);
			bendView_->setMirror(0);
		}

		mirror_ = newMirror;

		if (mirror_) {
			mirrorEditor_->setMirror(mirror_);
			upperSlitEditor_->setControl(mirror_->upperSlitBladeMotorControl());
			bendView_->setMirror(mirror_);
		}

		emit mirrorChanged(mirror_);
	}
}
