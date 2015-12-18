#include "BioXASSSRLMonochromatorConfigurationView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorEnergyView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorCrystalsView.h"
#include "ui/CLS/CLSMAXvMotorConfigurationView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create motors view.

	upperSlitEditor_ = new AMExtendedControlEditor(0);
	upperSlitEditor_->setTitle("Upper slit blade");
	upperSlitEditor_->setControlFormat('f', 3);

	lowerSlitEditor_ = new AMExtendedControlEditor(0);
	lowerSlitEditor_->setTitle("Lower slit blade");
	lowerSlitEditor_->setControlFormat('f', 3);

	heightEditor_ = new AMExtendedControlEditor(0);
	heightEditor_->setTitle("Height");
	heightEditor_->setControlFormat('f', 3);

	lateralEditor_ = new AMExtendedControlEditor(0);
	lateralEditor_->setTitle("Lateral");
	lateralEditor_->setControlFormat('f', 3);

	paddleEditor_ = new AMExtendedControlEditor(0);
	paddleEditor_->setTitle("Paddle");

	QVBoxLayout *motorsViewLayout = new QVBoxLayout();
	motorsViewLayout->addWidget(upperSlitEditor_);
	motorsViewLayout->addWidget(lowerSlitEditor_);
	motorsViewLayout->addWidget(heightEditor_);
	motorsViewLayout->addWidget(lateralEditor_);
	motorsViewLayout->addWidget(paddleEditor_);

	QGroupBox *motorsView = new QGroupBox("Motors");
	motorsView->setLayout(motorsViewLayout);
	motorsView->setMinimumWidth(VIEW_WIDTH_MIN);

	// Create energy view.

	energyView_ = new BioXASSSRLMonochromatorEnergyView(0);

	QVBoxLayout *energyBoxLayout = new QVBoxLayout();
	energyBoxLayout->addWidget(energyView_);

	QGroupBox *energyBox = new QGroupBox();
	energyBox->setTitle("Energy");
	energyBox->setLayout(energyBoxLayout);
	energyBox->setMinimumWidth(VIEW_WIDTH_MIN);

	// Create bragg config view

	braggConfigurationView_ = new CLSMAXvMotorConfigurationView(0);

	QVBoxLayout *braggBoxLayout = new QVBoxLayout();
	braggBoxLayout->setMargin(0);
	braggBoxLayout->addWidget(braggConfigurationView_);

	QGroupBox *braggBox = new QGroupBox("Goniometer configuration");
	braggBox->setLayout(braggBoxLayout);

	// Create crystals view

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Region");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	crystalsView_ = new BioXASSSRLMonochromatorCrystalsView(0);

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	QGroupBox *regionStatusView = new QGroupBox("Region Status");
	regionStatusView->setFlat(true);
	regionStatusView->setLayout(regionStatusViewLayout);

	QVBoxLayout *crystalsBoxLayout = new QVBoxLayout();
	crystalsBoxLayout->addWidget(regionEditor_);
	crystalsBoxLayout->addWidget(regionStatusView);
	crystalsBoxLayout->addWidget(crystalsView_);

	QGroupBox *crystalsBox = new QGroupBox("Crystals");
	crystalsBox->setLayout(crystalsBoxLayout);
	crystalsBox->setMinimumWidth(VIEW_WIDTH_MIN);

	// Main layouts

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addWidget(motorsView);
	leftLayout->addStretch();

	QVBoxLayout *centerLayout = new QVBoxLayout();
	centerLayout->addWidget(energyBox);
	centerLayout->addWidget(braggBox);
	centerLayout->addStretch();

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(crystalsBox);
	rightLayout->addStretch();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(leftLayout);
	layout->addLayout(centerLayout);
	layout->addLayout(rightLayout);

	setLayout(layout);

	// Current settings

	setMono(mono);
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::refresh()
{
	// Clear UI elements.

	upperSlitEditor_->setControl(0);
	lowerSlitEditor_->setControl(0);
	heightEditor_->setControl(0);
	lateralEditor_->setControl(0);
	paddleEditor_->setControl(0);

	regionEditor_->setControl(0);
	regionStatusWidget_->setRegionControl(0);
	crystalsView_->setMono(0);

	energyView_->setMono(0);

	braggConfigurationView_->setMotor(0);

	// Update view elements.

	if (mono_) {

		upperSlitEditor_->setControl(mono_->upperSlit());
		lowerSlitEditor_->setControl(mono_->lowerSlit());
		heightEditor_->setControl(mono_->vertical());
		lateralEditor_->setControl(mono_->lateral());
		paddleEditor_->setControl(mono_->paddle());

		regionEditor_->setControl(mono_->region());
		regionStatusWidget_->setRegionControl(mono_->region());
		crystalsView_->setMono(mono_);

		energyView_->setMono(mono_);

		braggConfigurationView_->setMotor(mono_->bragg());
	}
}

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newMono;

		if (mono_) {
			connect( mono_, SIGNAL(braggChanged(CLSMAXvMotor*)), this, SLOT(updateBraggConfigurationView()) );
		}

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::updateBraggConfigurationView()
{
	CLSMAXvMotor *braggControl = 0;

	if (mono_)
		braggControl = mono_->bragg();

	braggConfigurationView_->setMotor(braggControl);
}
