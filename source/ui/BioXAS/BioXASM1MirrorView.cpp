#include "BioXASM1MirrorView.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"

BioXASM1MirrorView::BioXASM1MirrorView(BioXASM1Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create stop button.

	stopButton_ = new AMControlStopButton(0);

	// Create mask view.

	maskView_ = new BioXASM1MirrorMaskView(0);

	QVBoxLayout *maskBoxLayout = new QVBoxLayout();
	maskBoxLayout->addWidget(maskView_);

	QGroupBox *maskBox = new QGroupBox();
	maskBox->setTitle("Mask");
	maskBox->setLayout(maskBoxLayout);

	// Create basic mirror view.

	mirrorView_ = new BioXASMirrorView(0);

	// Create and set layouts.

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(stopButton_);
	buttonLayout->addStretch();

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(buttonLayout);
	layout->addWidget(maskBox);
	layout->addWidget(mirrorView_);

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
	maskView_->setMirrorMask(0);
	mirrorView_->setMirror(0);

	// Update view elements.

	if (mirror_) {
		stopButton_->setControl(mirror_);
		updateMaskView();
		mirrorView_->setMirror(mirror_);
	}
}

void BioXASM1MirrorView::setMirror(BioXASM1Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_)
			connect( mirror_, SIGNAL(maskChanged(BioXASM1MirrorMask*)), this, SLOT(updateMaskView()) );

		refresh();

		emit mirrorChanged(mirror_);
	}
}

void BioXASM1MirrorView::updateMaskView()
{
	BioXASM1MirrorMask *maskControl = 0;

	if (mirror_)
		maskControl = mirror_->mask();

	maskView_->setMirrorMask(maskControl);
}
