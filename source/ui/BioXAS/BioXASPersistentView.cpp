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

	energyEditor_ = new AMExtendedControlEditor(0);
	energyEditor_->setTitle("Mono Energy (encoder)");
	energyEditor_->setControlFormat('f', 2);

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Mono Crystal Region");

	braggEditor_ = new AMExtendedControlEditor(0);
	braggEditor_->setTitle("Mono Goniometer Angle");
	braggEditor_->setControlFormat('f', 2);

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
	layout->addWidget(energyEditor_);
	layout->addWidget(regionEditor_);
	layout->addWidget(braggEditor_);
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
			energyEditor_->setControl(0);
			regionEditor_->setControl(0);
			braggEditor_->setControl(0);
		}

		mono_ = newMono;

		if (mono_) {
			energyEditor_->setControl(mono_->energyControl());
			regionEditor_->setControl(mono_->regionControl());
			braggEditor_->setControl(mono_->braggControl());
		}

		emit monoChanged(mono_);
	}
}

void BioXASPersistentView::setScalerChannelsVisible(bool show)
{
	channelsBox_->setVisible(show);
}
