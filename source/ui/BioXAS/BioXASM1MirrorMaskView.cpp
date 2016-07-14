#include "BioXASM1MirrorMaskView.h"
#include "ui/CLS/CLSControlEditor.h"
#include "beamline/BioXAS/BioXASMirrorMaskState.h"

BioXASM1MirrorMaskView::BioXASM1MirrorMaskView(BioXASM1Mirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mirror_ = 0;

	// Create UI elements.

	upperBladeEditor_ = new CLSControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");
	upperBladeEditor_->setFormat('f');
	upperBladeEditor_->setPrecision(5);

	stateEditor_ = new CLSControlEditor(0);
	stateEditor_->setTitle("State");

	// Create and set main layout.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(upperBladeEditor_);
	layout->addWidget(stateEditor_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASM1MirrorMaskView::~BioXASM1MirrorMaskView()
{

}

void BioXASM1MirrorMaskView::setMirror(BioXASM1Mirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( newMirror, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(maskUpperBladeChanged(AMControl*)), this, SLOT(updateUpperBladeEditor()) );
			connect( mirror_, SIGNAL(maskStateChanged(BioXASMirrorMaskState*)), this, SLOT(updateStateEditor()) );
		}

		emit mirrorChanged(mirror_);
	}

	updateUpperBladeEditor();
	updateStateEditor();
}

void BioXASM1MirrorMaskView::updateUpperBladeEditor()
{
	upperBladeEditor_->setControl(mirror_ ? mirror_->maskUpperBlade() : 0);
}

void BioXASM1MirrorMaskView::updateStateEditor()
{
	stateEditor_->setControl(mirror_ ? mirror_->maskState() : 0);
}
