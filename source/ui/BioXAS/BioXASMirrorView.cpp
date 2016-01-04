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

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
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

void BioXASMirrorView::refresh()
{
	// Clear the view.

	pitchEditor_->setControl(0);
	rollEditor_->setControl(0);
	yawEditor_->setControl(0);
	heightEditor_->setControl(0);
	lateralEditor_->setControl(0);

	// Update view elements.

	if (mirror_) {
		pitchEditor_->setControl(mirror_->pitch());
		rollEditor_->setControl(mirror_->roll());
		yawEditor_->setControl(mirror_->yaw());
		heightEditor_->setControl(mirror_->height());
		lateralEditor_->setControl(mirror_->lateral());
	}
}

void BioXASMirrorView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(pitchChanged(BioXASMirrorPitchControl*)), this, SLOT(refresh()) );
			connect( mirror_, SIGNAL(rollChanged(BioXASMirrorRollControl*)), this, SLOT(refresh()) );
			connect( mirror_, SIGNAL(yawChanged(BioXASMirrorYawControl*)), this, SLOT(refresh()) );
			connect( mirror_, SIGNAL(heightChanged(BioXASMirrorHeightControl*)), this, SLOT(refresh()) );
			connect( mirror_, SIGNAL(lateralChanged(BioXASMirrorLateralControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit mirrorChanged(mirror_);
	}
}
