#include "BioXASSSRLMonochromatorMaskView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/CLS/CLSSSRLMonochromatorMaskState.h"
#include "ui/CLS/CLSControlEditor.h"

BioXASSSRLMonochromatorMaskView::BioXASSSRLMonochromatorMaskView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create UI elements.

	stateEditor_ = new CLSControlEditor(0);
	stateEditor_->setTitle("State");

	upperBladeEditor_ = new CLSControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");
	upperBladeEditor_->setFormat('f');
	upperBladeEditor_->setPrecision(3);

	lowerBladeEditor_ = new CLSControlEditor(0);
	lowerBladeEditor_->setTitle("Lower blade");
	lowerBladeEditor_->setFormat('f');
	lowerBladeEditor_->setPrecision(3);

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

	setMono(mono);

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

void BioXASSSRLMonochromatorMaskView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newMono;

		if (mono_) {
			connect( mono_, SIGNAL(maskStateChanged(CLSSSRLMonochromatorMaskState*)), this, SLOT(updateStateEditor()) );
			connect( mono_, SIGNAL(maskUpperBladeChanged(BioXASMAXvMotor*)), this, SLOT(updateUpperBladeEditor()) );
			connect( mono_, SIGNAL(maskLowerBladeChanged(BioXASMAXvMotor*)), this, SLOT(updateLowerBladeEditor()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorMaskView::updateStateEditor()
{
	AMControl *stateControl = 0;

	if (mono_)
		stateControl = mono_->maskState();

	stateEditor_->setControl(stateControl);
}

void BioXASSSRLMonochromatorMaskView::updateUpperBladeEditor()
{
	AMControl *upperBladeControl = 0;

	if (mono_)
		upperBladeControl = mono_->maskUpperBlade();

	upperBladeEditor_->setControl(upperBladeControl);
}

void BioXASSSRLMonochromatorMaskView::updateLowerBladeEditor()
{
	AMControl *lowerBladeControl = 0;

	if (mono_)
		lowerBladeControl = mono_->maskLowerBlade();

	lowerBladeEditor_->setControl(lowerBladeControl);
}
