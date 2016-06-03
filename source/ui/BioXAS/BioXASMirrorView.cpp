#include "BioXASMirrorView.h"

BioXASMirrorView::BioXASMirrorView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	mirror_ = 0;

	// Create UI elements.

	pitchEditor_ = new BioXASControlSetpointEditor(0, true);
	pitchEditor_->setPrecision(3);

	rollEditor_ = new BioXASControlSetpointEditor(0, true);
	rollEditor_->setPrecision(3);

	heightEditor_ = new BioXASControlSetpointEditor(0, true);
	heightEditor_->setPrecision(3);

	yawEditor_ = new BioXASControlSetpointEditor(0, true);
	yawEditor_->setPrecision(3);

	lateralEditor_ = new BioXASControlSetpointEditor(0, true);
	lateralEditor_->setPrecision(3);

	bendEditor_ = new BioXASControlSetpointEditor(0, true);
	bendEditor_->setPrecision(3);

	moveButton_ = new QPushButton("Move");
	connect( moveButton_, SIGNAL(clicked(bool)), this, SLOT(onMoveButtonClicked()) );

	stopButton_ = new QPushButton("Stop");
	connect( stopButton_, SIGNAL(clicked(bool)), this, SLOT(onStopButtonClicked()) );

	// Create and set layouts.

	QGridLayout *controlsLayout = new QGridLayout();
	controlsLayout->setMargin(0);
	controlsLayout->addWidget(new QLabel("Pitch:"), 0, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(pitchEditor_, 0, 1);
	controlsLayout->addWidget(new QLabel("Roll:"), 1, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(rollEditor_, 1, 1);
	controlsLayout->addWidget(new QLabel("Height:"), 2, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(heightEditor_, 2, 1);
	controlsLayout->addWidget(new QLabel("Yaw:"), 3, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(yawEditor_, 3, 1);
	controlsLayout->addWidget(new QLabel("Lateral:"), 4, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(lateralEditor_, 4, 1);
	controlsLayout->addWidget(new QLabel("Bend:"), 5, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(bendEditor_, 5, 1);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setMargin(0);
	buttonLayout->addWidget(moveButton_);
	buttonLayout->addWidget(stopButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(controlsLayout);
	layout->addLayout(buttonLayout);

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

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(pitchChanged(BioXASMirrorPitchControl*)), this, SLOT(updatePitchEditor()) );
			connect( mirror_, SIGNAL(rollChanged(BioXASMirrorRollControl*)), this, SLOT(updateRollEditor()) );
			connect( mirror_, SIGNAL(yawChanged(BioXASMirrorYawControl*)), this, SLOT(updateYawEditor()) );
			connect( mirror_, SIGNAL(heightChanged(BioXASMirrorHeightControl*)), this, SLOT(updateHeightEditor()) );
			connect( mirror_, SIGNAL(lateralChanged(BioXASMirrorLateralControl*)), this, SLOT(updateLateralEditor()) );
			connect( mirror_, SIGNAL(bendChanged(BioXASMirrorBendControl*)), this, SLOT(updateBendEditor()) );
			connect( mirror_, SIGNAL(connected(bool)), this, SLOT(updateMoveButton()) );
			connect( mirror_, SIGNAL(connected(bool)), this, SLOT(updateStopButton()) );
		}

		emit mirrorChanged(mirror_);
	}

	updatePitchEditor();
	updateRollEditor();
	updateHeightEditor();
	updateYawEditor();
	updateLateralEditor();
	updateBendEditor();

	updateMoveButton();
	updateStopButton();
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

void BioXASMirrorView::updateBendEditor()
{
	AMControl *bendControl = 0;

	if (mirror_)
		bendControl = mirror_->bend();

	bendEditor_->setControl(bendControl);
}

void BioXASMirrorView::updateMoveButton()
{
	if (mirror_ && mirror_->isConnected() && mirror_->canMove())
		moveButton_->setEnabled(true);
	else
		moveButton_->setEnabled(false);
}

void BioXASMirrorView::updateStopButton()
{
	if (mirror_ && mirror_->isConnected() && mirror_->canStop())
		stopButton_->setEnabled(true);
	else
		stopButton_->setEnabled(false);
}

void BioXASMirrorView::onMoveButtonClicked()
{
	//if (mirror_)
		//mirror_->moveMirror(pitchEditor_->value(), rollEditor_->value(), heightEditor_->value(), yawEditor_->value(), lateralEditor_->value());
}

void BioXASMirrorView::onStopButtonClicked()
{
	if (mirror_)
		mirror_->stop();
}
