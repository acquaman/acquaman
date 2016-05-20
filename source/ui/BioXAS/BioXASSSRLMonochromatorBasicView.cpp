#include "BioXASSSRLMonochromatorBasicView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"

BioXASSSRLMonochromatorBasicView::BioXASSSRLMonochromatorBasicView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create UI elements.

	energyEditor_ = new BioXASControlEditor(0);
	energyEditor_->setTitle("Energy");
	energyEditor_->setFormat('f');

	braggAngleEditor_ = new BioXASControlEditor(0);
	braggAngleEditor_->setTitle("Goniometer angle");
	braggAngleEditor_->setFormat('f');
	braggAngleEditor_->setPrecision(3);

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Region");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(energyEditor_);
	layout->addWidget(braggAngleEditor_);
	layout->addWidget(regionEditor_);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorBasicView::~BioXASSSRLMonochromatorBasicView()
{

}

void BioXASSSRLMonochromatorBasicView::refresh()
{
	// Clear view.

	energyEditor_->setControl(0);
	braggAngleEditor_->setControl(0);
	regionEditor_->setControl(0);

	// Update view elements.

	updateEnergyEditor();
	updateBraggEditor();
	updateRegionEditor();
}

void BioXASSSRLMonochromatorBasicView::setMono(BioXASSSRLMonochromator *newControl)
{
	if (mono_ != newControl) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newControl;

		if (mono_) {
			connect( mono_, SIGNAL(energyChanged(BioXASMonochromatorEnergyControl*)), this, SLOT(updateEnergyEditor()) );
			connect( mono_, SIGNAL(braggChanged(CLSMAXvMotor*)), this, SLOT(updateBraggEditor()) );
			connect( mono_, SIGNAL(regionChanged(AMControl*)), this, SLOT(updateRegionEditor()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorBasicView::updateEnergyEditor()
{
	AMControl *energyControl = 0;

	if (mono_)
		energyControl = mono_->energy();

	energyEditor_->setControl(energyControl);
}

void BioXASSSRLMonochromatorBasicView::updateBraggEditor()
{
	AMControl *braggControl = 0;

	if (mono_)
		braggControl = mono_->bragg();

	braggAngleEditor_->setControl(braggControl);
}

void BioXASSSRLMonochromatorBasicView::updateRegionEditor()
{
	AMControl *regionControl = 0;

	if (mono_)
		regionControl = mono_->region();

	regionEditor_->setControl(regionControl);
}
