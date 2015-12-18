#include "BioXASSSRLMonochromatorConfigurationView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
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

	stepEnergyEditor_ = new AMExtendedControlEditor(0);
	stepEnergyEditor_->setTitle("Energy (step)");
	stepEnergyEditor_->setControlFormat('f', 2);

	encoderEnergyEditor_ = new AMExtendedControlEditor(0, 0, true);
	encoderEnergyEditor_->setTitle("Energy (encoder)");
	encoderEnergyEditor_->setControlFormat('f', 2);

	stepBraggEditor_ = new AMExtendedControlEditor(0);
	stepBraggEditor_->setTitle("Goniometer (step)");
	stepBraggEditor_->setControlFormat('f', 2);

	encoderBraggEditor_ = new AMExtendedControlEditor(0, 0, true);
	encoderBraggEditor_->setTitle("Goniometer (encoder)");
	encoderBraggEditor_->setControlFormat('f', 2);

	m1PitchEditor_ = new AMExtendedControlEditor(0);
	m1PitchEditor_->setTitle("M1 Mirror Pitch");
	m1PitchEditor_->setControlFormat('f', 2);

	QGridLayout *energyGridLayout = new QGridLayout();
	energyGridLayout->addWidget(stepEnergyEditor_, 0, 0);
	energyGridLayout->addWidget(encoderEnergyEditor_, 0, 1);
	energyGridLayout->addWidget(stepBraggEditor_, 1, 0);
	energyGridLayout->addWidget(encoderBraggEditor_, 1, 1);

	QVBoxLayout *energyViewLayout = new QVBoxLayout();
	energyViewLayout->addLayout(energyGridLayout);
	energyViewLayout->addWidget(m1PitchEditor_);

	QGroupBox *energyView = new QGroupBox("Energy");
	energyView->setLayout(energyViewLayout);
	energyView->setMinimumWidth(VIEW_WIDTH_MIN);

	// Create calibration view.

	calibrateEnergyButton_ = new QPushButton("Calibrate Energy");
	calibrateGoniometerButton_ = new QPushButton("Calibrate Goniometer");

	QHBoxLayout *calibrateButtonsLayout = new QHBoxLayout();
	calibrateButtonsLayout->addWidget(calibrateEnergyButton_);
	calibrateButtonsLayout->addWidget(calibrateGoniometerButton_);

	QGroupBox *calibrateButtonsView = new QGroupBox("Calibration");
	calibrateButtonsView->setLayout(calibrateButtonsLayout);

	// Create bragg config view

	braggConfigurationView_ = new CLSMAXvMotorConfigurationView(0);

	QVBoxLayout *braggConfigViewLayout = new QVBoxLayout();
	braggConfigViewLayout->setMargin(0);
	braggConfigViewLayout->addWidget(braggConfigurationView_);

	QGroupBox *braggConfigView = new QGroupBox("Goniometer configuration");
	braggConfigView->setLayout(braggConfigViewLayout);

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
	centerLayout->addWidget(energyView);
	centerLayout->addWidget(calibrateButtonsView);
	centerLayout->addWidget(braggConfigView);
	centerLayout->addStretch();

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(crystalsBox);
	rightLayout->addStretch();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(leftLayout);
	layout->addLayout(centerLayout);
	layout->addLayout(rightLayout);

	setLayout(layout);

	// Make connections

	connect( calibrateEnergyButton_, SIGNAL(clicked()), this, SLOT(onCalibrateEnergyButtonClicked()) );
	connect( calibrateGoniometerButton_, SIGNAL(clicked()), this, SLOT(onCalibrateGoniometerButtonClicked()) );

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

	stepEnergyEditor_->setControl(0);
	encoderEnergyEditor_->setControl(0);
	stepBraggEditor_->setControl(0);
	encoderBraggEditor_->setControl(0);
	m1PitchEditor_->setControl(0);

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

		stepEnergyEditor_->setControl(mono_->stepEnergy());
		encoderEnergyEditor_->setControl(mono_->encoderEnergy());
		stepBraggEditor_->setControl(mono_->stepBragg());
		encoderBraggEditor_->setControl(mono_->encoderBragg());
		m1PitchEditor_->setControl(mono_->m1MirrorPitch());

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

void BioXASSSRLMonochromatorConfigurationView::onCalibrateEnergyButtonClicked()
{
	if (mono_) {
		AMControl *energyControl = mono_->energy();

		if (energyControl) {
			bool inputOK = false;
			double oldEnergy = energyControl->value();
			double newEnergy = QInputDialog::getDouble(this, "Energy Calibration", "Enter calibrated energy:", oldEnergy, ENERGY_MIN, ENERGY_MAX, 2, &inputOK);

			if (inputOK)
				energyControl->calibrate(oldEnergy, newEnergy);
		}
	}
}

void BioXASSSRLMonochromatorConfigurationView::onCalibrateGoniometerButtonClicked()
{
	if (mono_) {
		AMControl *braggMotor = mono_->bragg();

		if (braggMotor) {
			bool inputOK = false;
			double oldPosition = braggMotor->value();
			double newPosition = QInputDialog::getDouble(this, "Goniometer Calibration", "Enter calibrated goniometer position:", oldPosition, BRAGG_POSITION_MIN, BRAGG_POSITION_MAX, 2, &inputOK);

			if (inputOK)
				braggMotor->calibrate(oldPosition, newPosition);
		}
	}
}

void BioXASSSRLMonochromatorConfigurationView::updateBraggConfigurationView()
{
	CLSMAXvMotor *braggControl = 0;

	if (mono_)
		braggControl = mono_->bragg();

	braggConfigurationView_->setMotor(braggControl);
}
