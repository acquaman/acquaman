#include "BioXASMirrorEditor.h"
#include "beamline/BioXAS/BioXASMirror.h"
#include "ui/BioXAS/BioXASValueSetpointEditor.h"

BioXASMirrorEditor::BioXASMirrorEditor(BioXASMirror *mirror, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	mirror_ = 0;

	// Create UI elements.

	pitchEditor_ = new BioXASValueSetpointEditor();
	rollEditor_ = new BioXASValueSetpointEditor();
	heightEditor_ = new BioXASValueSetpointEditor();
	yawEditor_ = new BioXASValueSetpointEditor();
	lateralEditor_ = new BioXASValueSetpointEditor();

	moveButton_ = new QPushButton("Move");
	stopButton_ = new QPushButton("Stop");

	// Create and set main layouts.

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(moveButton_);
	buttonLayout->addWidget(stopButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(pitchEditor_);
	layout->addWidget(rollEditor_);
	layout->addWidget(heightEditor_);
	layout->addWidget(yawEditor_);
	layout->addWidget(lateralEditor_);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	// Current settings.

	updatePitchEditor();
	updateRollEditor();
	updateHeightEditor();
	updateYawEditor();
	updateLateralEditor();
}

BioXASMirrorEditor::~BioXASMirrorEditor()
{

}

void BioXASMirrorEditor::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(pitchValueChanged(double)), this, SLOT(updatePitchEditor()) );
			connect( mirror_, SIGNAL(rollValueChanged(double)), this, SLOT(updateRollEditor()) );
			connect( mirror_, SIGNAL(heightValueChanged(double)), this, SLOT(updateHeightEditor()) );
			connect( mirror_, SIGNAL(yawValueChanged(double)), this, SLOT(updateYawEditor()) );
			connect( mirror_, SIGNAL(lateralValueChanged(double)), this, SLOT(updateLateralEditor()) );
		}

		updatePitchEditor();
		updateRollEditor();
		updateHeightEditor();
		updateYawEditor();
		updateLateralEditor();
	}
}

void BioXASMirrorEditor::updatePitchEditor()
{
	if (mirror_) {
		pitchEditor_->setEnabled(true);
		pitchEditor_->setValue(mirror_->pitchValue());

	} else {
		pitchEditor_->clear();
		pitchEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateRollEditor()
{
	if (mirror_) {
		rollEditor_->setEnabled(true);
		rollEditor_->setValue(mirror_->rollValue());

	} else {
		rollEditor_->clear();
		rollEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateHeightEditor()
{
	if (mirror_) {
		heightEditor_->setEnabled(true);
		heightEditor_->setValue(mirror_->heightValue());

	} else {
		heightEditor_->clear();
		heightEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateYawEditor()
{
	if (mirror_) {
		yawEditor_->setEnabled(true);
		yawEditor_->setValue(mirror_->yawValue());

	} else {
		yawEditor_->clear();
		yawEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateLateralEditor()
{
	if (mirror_) {
		lateralEditor_->setEnabled(true);
		lateralEditor_->setValue(mirror_->lateralValue());

	} else {
		lateralEditor_->clear();
		lateralEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::onMoveButtonClicked()
{
	if (mirror_) {
		mirror_->moveMirror()
	}
}

void BioXASMirrorEditor::onStopButtonClicked()
{
	if (mirror_) {

	}
}

