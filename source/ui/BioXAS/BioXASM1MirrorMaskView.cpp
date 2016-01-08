#include "BioXASM1MirrorMaskView.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"

BioXASM1MirrorMaskView::BioXASM1MirrorMaskView(BioXASM1MirrorMask *mask, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mask_ = 0;

	// Create UI elements.

	upperBladeEditor_ = new BioXASControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");

	stateEditor_ = new BioXASControlEditor(0);
	stateEditor_->setTitle("State");

	// Create and set main layout.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(upperBladeEditor_);
	layout->addWidget(stateEditor_);

	setLayout(layout);

	// Current settings.

	setMirrorMask(mask);

	refresh();
}

BioXASM1MirrorMaskView::~BioXASM1MirrorMaskView()
{

}

void BioXASM1MirrorMaskView::refresh()
{
	// Clear view.

	upperBladeEditor_->setControl(0);
	stateEditor_->setControl(0);

	// Update view elements.

	updateUpperBladeEditor();
	updateStateEditor();
}

void BioXASM1MirrorMaskView::setMirrorMask(BioXASM1MirrorMask *newControl)
{
	if (mask_ != newControl) {

		if (mask_)
			disconnect( mask_, 0, this, 0 );

		mask_ = newControl;

		if (mask_) {
			connect( mask_, SIGNAL(upperSlitBladeChanged(AMControl*)), this, SLOT(updateUpperBladeEditor()) );
			connect( mask_, SIGNAL(stateChanged(BioXASM1MirrorMaskState*)), this, SLOT(updateStateEditor()) );
		}

		refresh();

		emit maskChanged(mask_);
	}
}

void BioXASM1MirrorMaskView::updateUpperBladeEditor()
{
	AMControl *upperBladeControl = 0;

	if (mask_)
		upperBladeControl = mask_->upperSlitBlade();

	upperBladeEditor_->setControl(upperBladeControl);
}

void BioXASM1MirrorMaskView::updateStateEditor()
{
	BioXASM1MirrorMaskState *stateControl = 0;

	if (mask_)
		stateControl = mask_->state();

	stateEditor_->setControl(stateControl);
}
