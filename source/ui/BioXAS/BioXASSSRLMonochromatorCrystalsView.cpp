#include "BioXASSSRLMonochromatorCrystalsView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSSRLMonochromatorCrystalsView::BioXASSSRLMonochromatorCrystalsView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create crystal 1 view.

	crystal1PitchEditor_ = new AMExtendedControlEditor(0);
	crystal1PitchEditor_->setTitle("Pitch motor");

	crystal1RollEditor_ = new AMExtendedControlEditor(0);
	crystal1RollEditor_->setTitle("Roll motor");

	QVBoxLayout *crystal1BoxLayout = new QVBoxLayout();
	crystal1BoxLayout->addWidget(crystal1PitchEditor_);
	crystal1BoxLayout->addWidget(crystal1RollEditor_);

	QGroupBox *crystal1Box = new QGroupBox();
	crystal1Box->setTitle("Crystal A"); // Right now, crystals are labelled by region instead of number.
	crystal1Box->setFlat(true);
	crystal1Box->setLayout(crystal1BoxLayout);

	// Create crystal 2 view.

	crystal2PitchEditor_ = new AMExtendedControlEditor(0);
	crystal2PitchEditor_->setTitle("Pitch motor");

	crystal2RollEditor_ = new AMExtendedControlEditor(0);
	crystal2RollEditor_->setTitle("Roll motor");

	QVBoxLayout *crystal2BoxLayout = new QVBoxLayout();
	crystal2BoxLayout->addWidget(crystal2PitchEditor_);
	crystal2BoxLayout->addWidget(crystal2RollEditor_);

	QGroupBox *crystal2Box = new QGroupBox();
	crystal2Box->setTitle("Crystal B");
	crystal2Box->setFlat(true);
	crystal2Box->setLayout(crystal2BoxLayout);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(crystal1Box);
	layout->addWidget(crystal2Box);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorCrystalsView::~BioXASSSRLMonochromatorCrystalsView()
{

}

void BioXASSSRLMonochromatorCrystalsView::refresh()
{
	// Clear view.

	crystal1PitchEditor_->setControl(0);
	crystal1RollEditor_->setControl(0);
	crystal2PitchEditor_->setControl(0);
	crystal2RollEditor_->setControl(0);

	// Update view elements.

	updateCrystal1PitchEditor();
	updateCrystal1RollEditor();
	updateCrystal2PitchEditor();
	updateCrystal2RollEditor();
}

void BioXASSSRLMonochromatorCrystalsView::setMono(BioXASSSRLMonochromator *newControl)
{
	if (mono_ != newControl) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newControl;

		if (mono_) {
			connect( mono_, SIGNAL(crystal1PitchChanged(CLSMAXvMotor*)), this, SLOT(updateCrystal1PitchEditor()) );
			connect( mono_, SIGNAL(crystal1RollChanged(CLSMAXvMotor*)), this, SLOT(updateCrystal1RollEditor()) );
			connect( mono_, SIGNAL(crystal2PitchChanged(CLSMAXvMotor*)), this, SLOT(updateCrystal2PitchEditor()) );
			connect( mono_, SIGNAL(crystal2RollChanged(CLSMAXvMotor*)), this, SLOT(updateCrystal2RollEditor()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorCrystalsView::updateCrystal1PitchEditor()
{
	AMControl *pitchControl = 0;

	if (mono_)
		pitchControl = mono_->crystal1Pitch();

	crystal1PitchEditor_->setControl(pitchControl);
}

void BioXASSSRLMonochromatorCrystalsView::updateCrystal1RollEditor()
{
	AMControl *rollControl = 0;

	if (mono_)
		rollControl = mono_->crystal1Roll();

	crystal1RollEditor_->setControl(rollControl);
}

void BioXASSSRLMonochromatorCrystalsView::updateCrystal2PitchEditor()
{
	AMControl *pitchControl = 0;

	if (mono_)
		pitchControl = mono_->crystal2Pitch();

	crystal2PitchEditor_->setControl(pitchControl);
}

void BioXASSSRLMonochromatorCrystalsView::updateCrystal2RollEditor()
{
	AMControl *rollControl = 0;

	if (mono_)
		rollControl = mono_->crystal2Roll();

	crystal2RollEditor_->setControl(rollControl);
}
