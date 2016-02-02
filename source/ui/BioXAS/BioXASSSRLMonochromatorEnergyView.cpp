#include "BioXASSSRLMonochromatorEnergyView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/CLS/CLSMAXvMotorConfigurationView.h"

BioXASSSRLMonochromatorEnergyView::BioXASSSRLMonochromatorEnergyView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create energy editors.

	stepEnergyEditor_ = new BioXASControlEditor(0);
	stepEnergyEditor_->setTitle("Energy (step)");
	stepEnergyEditor_->setFormat('f');

	encoderEnergyEditor_ = new BioXASControlEditor(0);
	encoderEnergyEditor_->setTitle("Energy (encoder)");
	encoderEnergyEditor_->setFormat('f');
	encoderEnergyEditor_->setReadOnly(true);

	stepBraggEditor_ = new BioXASControlEditor(0);
	stepBraggEditor_->setTitle("Goniometer (step)");
	stepBraggEditor_->setFormat('f');
	stepBraggEditor_->setPrecision(3);

	encoderBraggEditor_ = new BioXASControlEditor(0);
	encoderBraggEditor_->setTitle("Goniometer (encoder)");
	encoderBraggEditor_->setFormat('f');
	encoderBraggEditor_->setPrecision(3);
	encoderBraggEditor_->setReadOnly(true);

	mirrorPitchEditor_ = new BioXASControlEditor(0);
	mirrorPitchEditor_->setTitle("M1 mirror pitch");
	mirrorPitchEditor_->setFormat('f');
	mirrorPitchEditor_->setPrecision(3);

	// Create bragg configuration view.

	braggConfigurationView_ = new CLSMAXvMotorConfigurationView(0);

	QVBoxLayout *braggConfigBoxLayout = new QVBoxLayout();
	braggConfigBoxLayout->addWidget(braggConfigurationView_);

	QGroupBox *braggConfigBox = new QGroupBox();
	braggConfigBox->setTitle("Goniometer Configuration");
	braggConfigBox->setFlat(true);
	braggConfigBox->setLayout(braggConfigBoxLayout);

	// Create and set main layout.

	QGridLayout *energyEditorsLayout = new QGridLayout();
	energyEditorsLayout->addWidget(stepEnergyEditor_, 0, 0);
	energyEditorsLayout->addWidget(encoderEnergyEditor_, 0, 1);
	energyEditorsLayout->addWidget(stepBraggEditor_, 1, 0);
	energyEditorsLayout->addWidget(encoderBraggEditor_, 1, 1);
	energyEditorsLayout->addWidget(mirrorPitchEditor_, 2, 0, 1, 2);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(energyEditorsLayout);
	layout->addWidget(braggConfigBox);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorEnergyView::~BioXASSSRLMonochromatorEnergyView()
{

}

void BioXASSSRLMonochromatorEnergyView::refresh()
{
	updateStepEnergyEditor();
	updateEncoderEnergyEditor();
	updateStepBraggEditor();
	updateEncoderBraggEditor();
	updateMirrorPitchEditor();
	updateBraggConfigurationView();
}

void BioXASSSRLMonochromatorEnergyView::setMono(BioXASSSRLMonochromator *newControl)
{
	if (mono_ != newControl) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newControl;

		if (mono_) {
			connect( mono_, SIGNAL(stepEnergyChanged(BioXASSSRLMonochromatorEnergyControl*)), this, SLOT(updateStepEnergyEditor()) );
			connect( mono_, SIGNAL(encoderEnergyChanged(BioXASSSRLMonochromatorEnergyControl*)), this, SLOT(updateEncoderEnergyEditor()) );
			connect( mono_, SIGNAL(stepBraggChanged(CLSMAXvMotor*)), this, SLOT(updateStepBraggEditor()) );
			connect( mono_, SIGNAL(encoderBraggChanged(CLSMAXvMotor*)), this, SLOT(updateEncoderBraggEditor()) );
			connect( mono_, SIGNAL(m1MirrorPitchControlChanged(AMControl*)), this, SLOT(updateMirrorPitchEditor()) );
			connect( mono_, SIGNAL(braggChanged(CLSMAXvMotor*)), this, SLOT(updateBraggConfigurationView()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorEnergyView::updateStepEnergyEditor()
{
	AMControl *energyControl = 0;

	if (mono_)
		energyControl = mono_->stepEnergy();

	stepEnergyEditor_->setControl(energyControl);
}

void BioXASSSRLMonochromatorEnergyView::updateEncoderEnergyEditor()
{
	AMControl *energyControl = 0;

	if (mono_)
		energyControl = mono_->encoderEnergy();

	encoderEnergyEditor_->setControl(energyControl);
}


void BioXASSSRLMonochromatorEnergyView::updateStepBraggEditor()
{
	AMControl *braggControl = 0;

	if (mono_)
		braggControl = mono_->stepBragg();

	stepBraggEditor_->setControl(braggControl);
}

void BioXASSSRLMonochromatorEnergyView::updateEncoderBraggEditor()
{
	AMControl *braggControl = 0;

	if (mono_)
		braggControl = mono_->encoderBragg();

	encoderBraggEditor_->setControl(braggControl);
}

void BioXASSSRLMonochromatorEnergyView::updateMirrorPitchEditor()
{
	AMControl *pitchControl = 0;

	if (mono_)
		pitchControl = mono_->m1MirrorPitch();

	mirrorPitchEditor_->setControl(pitchControl);
}

void BioXASSSRLMonochromatorEnergyView::updateBraggConfigurationView()
{
	CLSMAXvMotor *braggMotor = 0;

	if (mono_)
		braggMotor = mono_->bragg();

	braggConfigurationView_->setMotor(braggMotor);
}
