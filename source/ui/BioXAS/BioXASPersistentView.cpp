#include "BioXASPersistentView.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

BioXASPersistentView::BioXASPersistentView(BioXASSSRLMonochromator *mono, CLSSIS3820Scaler *scaler, QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	encoderEnergyEditor_ = new AMExtendedControlEditor(0);
	encoderEnergyEditor_->setTitle("Mono Energy (encoder)");
	encoderEnergyEditor_->setControlFormat('f', 2);

	stepEnergyEditor_ = new AMExtendedControlEditor(0);
	stepEnergyEditor_->setTitle("Mono Energy (step)");
	stepEnergyEditor_->setControlFormat('f', 2);

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Mono Crystal Region");

	encoderBraggEditor_ = new AMExtendedControlEditor(0);
	encoderBraggEditor_->setTitle("Mono Goniometer Angle (encoder)");
	encoderBraggEditor_->setControlFormat('f', 2);

	stepBraggEditor_ = new AMExtendedControlEditor(0);
	stepBraggEditor_->setTitle("Mono Goniometer Angle (step)");
	stepBraggEditor_->setControlFormat('f', 2);

	// Create the scaler channel views.

	BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(scaler);

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channelsView);

	channelsBox_ = new QGroupBox();
	channelsBox_->setTitle("Scaler channels");
	channelsBox_->setLayout(channelsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(encoderEnergyEditor_);
	layout->addWidget(stepEnergyEditor_);
	layout->addWidget(regionEditor_);
	layout->addWidget(encoderBraggEditor_);
	layout->addWidget(stepBraggEditor_);
	layout->addWidget(channelsBox_);

	setLayout(layout);

	// Initial settings.

	channelsBox_->hide();

	// Current settings.

	setMono(mono);
}

BioXASPersistentView::~BioXASPersistentView()
{

}

void BioXASPersistentView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_) {
			encoderEnergyEditor_->setControl(0);
			stepEnergyEditor_->setControl(0);
			regionEditor_->setControl(0);
			encoderBraggEditor_->setControl(0);
			stepBraggEditor_->setControl(0);
		}

		mono_ = newMono;

		if (mono_) {
			encoderEnergyEditor_->setControl(mono_->encoderEnergyControl());
			stepEnergyEditor_->setControl(mono_->stepEnergyControl());
			regionEditor_->setControl(mono_->regionControl());
			encoderBraggEditor_->setControl(mono_->braggControl());
			stepBraggEditor_->setControl(mono_->stepBraggControl());
		}

		emit monoChanged(mono_);
	}
}

void BioXASPersistentView::setScalerChannelsVisible(bool show)
{
	channelsBox_->setVisible(show);
}
