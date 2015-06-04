#include "BioXASPersistentView.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

BioXASPersistentView::BioXASPersistentView(AMControl *energyControl, BioXASSSRLMonochromatorRegionControl *regionControl, AMControl *braggControl, CLSSIS3820Scaler *scaler, QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	energyControlEditor_ = new AMExtendedControlEditor(energyControl);
	energyControlEditor_->setTitle("Mono Energy");
	energyControlEditor_->setControlFormat('f', 2);

	regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(regionControl);
	regionControlEditor_->setTitle("Mono Crystal Region");

	braggControlEditor_ = new AMExtendedControlEditor(braggControl);
	braggControlEditor_->setTitle("Mono Goniometer Angle");
	braggControlEditor_->setControlFormat('f', 2);

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
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(channelsBox_);

	setLayout(layout);

	// Initial settings.

	channelsBox_->hide();
}

BioXASPersistentView::~BioXASPersistentView()
{

}

void BioXASPersistentView::setScalerChannelsVisible(bool show)
{
	channelsBox_->setVisible(show);
}
