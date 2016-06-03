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

	setMirror(mirror);
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

		emit mirrorChanged(mirror_);
	}

	updatePitchEditor();
	updateRollEditor();
	updateHeightEditor();
	updateYawEditor();
	updateLateralEditor();

	updateMoveButton();
	updateStopButton();
}

void BioXASMirrorEditor::updatePitchEditor()
{
	if (mirror_ && mirror_->pitch()) {
		pitchEditor_->setEnabled(true);
		pitchEditor_->setValue(mirror_->pitch()->setpoint());
		pitchEditor_->setDisplayFeedbackValue(true);
		pitchEditor_->setFeedbackValue(mirror_->pitch()->value());
		pitchEditor_->setMinimum(mirror_->pitch()->minimumValue());
		pitchEditor_->setMaximum(mirror_->pitch()->maximumValue());

	} else {
		pitchEditor_->clear();
		pitchEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateRollEditor()
{
	if (mirror_ && mirror_->roll()) {
		rollEditor_->setEnabled(true);
		rollEditor_->setValue(mirror_->roll()->setpoint());
		rollEditor_->setDisplayFeedbackValue(true);
		rollEditor_->setFeedbackValue(mirror_->roll()->value());
		rollEditor_->setMinimum(mirror_->roll()->minimumValue());
		rollEditor_->setMaximum(mirror_->roll()->maximumValue());

	} else {
		rollEditor_->clear();
		rollEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateHeightEditor()
{
	if (mirror_ && mirror_->height()) {
		heightEditor_->setEnabled(true);
		heightEditor_->setValue(mirror_->height()->setpoint());
		heightEditor_->setDisplayFeedbackValue(true);
		heightEditor_->setFeedbackValue(mirror_->height()->value());
		heightEditor_->setMinimum(mirror_->height()->minimumValue());
		heightEditor_->setMaximum(mirror_->height()->maximumValue());

	} else {
		heightEditor_->clear();
		heightEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateYawEditor()
{
	if (mirror_ && mirror_->yaw()) {
		yawEditor_->setEnabled(true);
		yawEditor_->setValue(mirror_->yaw()->setpoint());
		yawEditor_->setDisplayFeedbackValue(true);
		yawEditor_->setFeedbackValue(mirror_->yaw()->value());
		yawEditor_->setMinimum(mirror_->yaw()->minimumValue());
		yawEditor_->setMaximum(mirror_->yaw()->maximumValue());

	} else {
		yawEditor_->clear();
		yawEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateLateralEditor()
{
	if (mirror_ && mirror_->lateral()) {
		lateralEditor_->setEnabled(true);
		lateralEditor_->setValue(mirror_->lateral()->setpoint());
		lateralEditor_->setDisplayFeedbackValue(true);
		lateralEditor_->setFeedbackValue(mirror_->lateral()->value());
		lateralEditor_->setMinimum(mirror_->lateral()->minimumValue());
		lateralEditor_->setMaximum(mirror_->lateral()->maximumValue());

	} else {
		lateralEditor_->clear();
		lateralEditor_->setEnabled(false);
	}
}

void BioXASMirrorEditor::updateMoveButton()
{
	if (mirror_ && mirror_->isConnected())
		moveButton_->setEnabled(true);
	else
		moveButton_->setEnabled(false);
}

void BioXASMirrorEditor::updateStopButton()
{
	if (mirror_ && mirror_->isConnected())
		stopButton_->setEnabled(true);
	else
		stopButton_->setEnabled(false);
}

void BioXASMirrorEditor::onMoveButtonClicked()
{
	if (mirror_)
		mirror_->moveMirror(pitchEditor_->value(), rollEditor_->value(), heightEditor_->value(), yawEditor_->value(), lateralEditor_->value());
}

void BioXASMirrorEditor::onStopButtonClicked()
{
	if (mirror_)
		mirror_->stop();
}

