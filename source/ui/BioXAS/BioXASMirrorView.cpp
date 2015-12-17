#include "BioXASMirrorView.h"
#include "beamline/BioXAS/BioXASMirror.h"
#include "ui/BioXAS/BioXASMirrorBendView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASMirrorView::BioXASMirrorView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create basic controls view.

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

	QVBoxLayout *controlsBoxLayout = new QVBoxLayout();
	controlsBoxLayout->addWidget(pitchEditor_);
	controlsBoxLayout->addWidget(rollEditor_);
	controlsBoxLayout->addWidget(yawEditor_);
	controlsBoxLayout->addWidget(heightEditor_);
	controlsBoxLayout->addWidget(lateralEditor_);

	QGroupBox *controlsBox = new QGroupBox();
	controlsBox->setTitle("Mirror");
	controlsBox->setLayout(controlsBoxLayout);
	controlsBox->setMinimumWidth(350);

	// Create bend view.

	bendView_ = new BioXASMirrorBendView(0);

	QVBoxLayout *bendBoxLayout = new QVBoxLayout();
	bendBoxLayout->addWidget(bendView_);
	bendBoxLayout->addStretch();

	QGroupBox *bendBox = new QGroupBox();
	bendBox->setTitle("Bend");
	bendBox->setLayout(bendBoxLayout);
	bendBox->setMinimumWidth(350);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(controlsBox);
	layout->addWidget(bendBox);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);

	refresh();
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

	bendView_->setMirror(0);

	// Update view elements.

	updatePitchEditor();
	updateRollEditor();
	updateYawEditor();
	updateHeightEditor();
	updateLateralEditor();

	bendView_->setMirror(mirror_);
}

void BioXASMirrorView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(pitchChanged(BioXASMirrorPitchControl*)), this, SLOT(updatePitchEditor()) );
			connect( mirror_, SIGNAL(rollChanged(BioXASMirrorRollControl*)), this, SLOT(updateRollEditor()) );
			connect( mirror_, SIGNAL(yawChanged(BioXASMirrorYawControl*)), this, SLOT(updateYawEditor()) );
			connect( mirror_, SIGNAL(heightChanged(BioXASMirrorHeightControl*)), this, SLOT(updateHeightEditor()) );
			connect( mirror_, SIGNAL(lateralChanged(BioXASMirrorLateralControl*)), this, SLOT(updateLateralEditor()) );
		}

		refresh();

		emit mirrorChanged(mirror_);
	}
}

void BioXASMirrorView::updatePitchEditor()
{
	AMControl *pitchControl = 0;

	if (mirror_)
		pitchControl = mirror_->pitch();

	pitchEditor_->setControl(pitchControl);
}

void BioXASMirrorView::updateRollEditor()
{
	AMControl *rollControl = 0;

	if (mirror_)
		rollControl = mirror_->roll();

	rollEditor_->setControl(rollControl);
}

void BioXASMirrorView::updateYawEditor()
{
	AMControl *yawControl = 0;

	if (mirror_)
		yawControl = mirror_->yaw();

	yawEditor_->setControl(yawControl);
}

void BioXASMirrorView::updateHeightEditor()
{
	AMControl *heightControl = 0;

	if (mirror_)
		heightControl = mirror_->height();

	heightEditor_->setControl(heightControl);
}

void BioXASMirrorView::updateLateralEditor()
{
	AMControl *lateralControl = 0;

	if (mirror_)
		lateralControl = mirror_->lateral();

	lateralEditor_->setControl(lateralControl);
}
