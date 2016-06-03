#include "BioXASM1MirrorView.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"

BioXASM1MirrorView::BioXASM1MirrorView(BioXASM1Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	mirror_ = 0;

	// The mask blade editor.

	maskBladeEditor_ = new CLSControlEditor(0);
	maskBladeEditor_->setTitle("Mask upper blade");

	// The basic mirror view.

	basicView_ = new BioXASMirrorView(0);

	QVBoxLayout *basicViewBoxLayout = new QVBoxLayout();
	basicViewBoxLayout->addWidget(basicView_);

	QGroupBox *basicViewBox = new QGroupBox("Mirror");
	basicViewBox->setLayout(basicViewBoxLayout);

	// The downstream blade current editor.

	downstreamBladeCurrentEditor_ = new CLSControlEditor(0);
	downstreamBladeCurrentEditor_->setTitle("Downstream blade current");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(maskBladeEditor_);
	layout->addWidget(basicViewBox);
	layout->addWidget(downstreamBladeCurrentEditor_);

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
		mirror_ = newMirror;
		emit mirrorChanged(mirror_);
	}

	updateMaskBladeEditor();
	updateBasicView();
	updateDownstreamBladeCurrentEditor();
}

void BioXASM1MirrorView::updateMaskBladeEditor()
{
	AMControl *bladeControl = 0;

	if (mirror_ && mirror_->mask())
		bladeControl = mirror_->mask()->upperSlitBlade();

	maskBladeEditor_->setControl(bladeControl);
}

void BioXASM1MirrorView::updateBasicView()
{
	basicView_->setMirror(mirror_);
}

void BioXASM1MirrorView::updateDownstreamBladeCurrentEditor()
{
	AMControl *downstreamBladeCurrentControl = 0;

	if (mirror_)
		downstreamBladeCurrentControl = mirror_->downstreamBladeCurrent();

	downstreamBladeCurrentEditor_->setControl(downstreamBladeCurrentControl);
}
