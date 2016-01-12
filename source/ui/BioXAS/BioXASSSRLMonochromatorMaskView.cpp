#include "BioXASSSRLMonochromatorMaskView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASSSRLMonochromatorMaskView::BioXASSSRLMonochromatorMaskView(BioXASSSRLMonochromatorMask *mask, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mask_ = 0;

	// Create UI elements.

	stateEditor_ = new BioXASControlEditor(0);
	stateEditor_->setTitle("State");

	upperBladeEditor_ = new BioXASControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");

	lowerBladeEditor_ = new BioXASControlEditor(0);
	lowerBladeEditor_->setTitle("Lower blade");

	// Create and set layouts.

	QHBoxLayout *bladesLayout = new QHBoxLayout();
	bladesLayout->addWidget(upperBladeEditor_);
	bladesLayout->addWidget(lowerBladeEditor_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(stateEditor_);
	layout->addLayout(bladesLayout);

	setLayout(layout);

	// Current settings.

	setMask(mask);

	refresh();
}

BioXASSSRLMonochromatorMaskView::~BioXASSSRLMonochromatorMaskView()
{

}

void BioXASSSRLMonochromatorMaskView::refresh()
{
	updateStateEditor();
	updateUpperBladeEditor();
	updateLowerBladeEditor();
}

void BioXASSSRLMonochromatorMaskView::setMask(BioXASSSRLMonochromatorMask *newMask)
{
	if (mask_ != newMask) {

		if (mask_)
			disconnect( mask_, 0, this, 0 );

		mask_ = newMask;

		if (mask_) {
			connect( mask_, SIGNAL(stateChanged(BioXASSSRLMonochromatorMaskState*)), this, SLOT(updateStateEditor()) );
			connect( mask_, SIGNAL(upperBladeChanged(AMControl*)), this, SLOT(updateUpperBladeEditor()) );
			connect( mask_, SIGNAL(lowerBladeChanged(AMControl*)), this, SLOT(updateLowerBladeEditor()) );
		}

		refresh();

		emit maskChanged(mask_);
	}
}

void BioXASSSRLMonochromatorMaskView::updateStateEditor()
{
	AMControl *stateControl = 0;

	if (mask_)
		stateControl = mask_->state();

	stateEditor_->setControl(stateControl);
}

void BioXASSSRLMonochromatorMaskView::updateUpperBladeEditor()
{
	AMControl *upperBladeControl = 0;

	if (mask_)
		upperBladeControl = mask_->upperBlade();

	upperBladeEditor_->setControl(upperBladeControl);
}

void BioXASSSRLMonochromatorMaskView::updateLowerBladeEditor()
{
	AMControl *lowerBladeControl = 0;

	if (mask_)
		lowerBladeControl = mask_->lowerBlade();

	lowerBladeEditor_->setControl(lowerBladeControl);
}
