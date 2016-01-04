#include "BioXASM1MirrorView.h"

BioXASM1MirrorView::BioXASM1MirrorView(BioXASM1Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create UI elements.

	stopButton_ = new AMControlStopButton(0);

	mirrorEditor_ = new BioXASMirrorView(0);

	upperSlitEditor_ = new AMExtendedControlEditor(0);
	upperSlitEditor_->setTitle("Upper slit blade");

	bendView_ = new BioXASMirrorBendView(0);

	// Create and set layouts.

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(stopButton_);
	buttonLayout->addStretch();

	QVBoxLayout *controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(mirrorEditor_);
	controlsLayout->addWidget(upperSlitEditor_);

	QVBoxLayout *bendLayout = new QVBoxLayout();
	bendLayout->addWidget(bendView_);
	bendLayout->addStretch();

	QHBoxLayout *mirrorLayout = new QHBoxLayout();
	mirrorLayout->addLayout(controlsLayout);
	mirrorLayout->addLayout(bendLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(buttonLayout);
	layout->addLayout(mirrorLayout);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASM1MirrorView::~BioXASM1MirrorView()
{

}

void BioXASM1MirrorView::refresh()
{
	// Clear view.

	stopButton_->setControl(0);
	mirrorEditor_->setMirror(0);
	upperSlitEditor_->setControl(0);
	bendView_->setMirror(0);

	// Update view elements.

	if (mirror_) {
		stopButton_->setControl(mirror_);
		mirrorEditor_->setMirror(mirror_);
		bendView_->setMirror(mirror_);

		updateUpperSlitEditor();
	}
}

void BioXASM1MirrorView::setMirror(BioXASM1Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_)
			connect( mirror_, SIGNAL(upperSlitBladeMotorChanged(CLSMAXvMotor*)), this, SLOT(updateUpperSlitEditor()) );

		refresh();

		emit mirrorChanged(mirror_);
	}
}

void BioXASM1MirrorView::updateUpperSlitEditor()
{
	AMControl *upperSlitControl = 0;

	if (mirror_)
		upperSlitControl = mirror_->upperSlitBladeMotor();

	upperSlitEditor_->setControl(upperSlitControl);
}
