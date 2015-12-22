#include "BioXASSSRLMonochromatorConfigurationView.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create UI elements.

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

	crystal1PitchEditor_ = new AMExtendedControlEditor(0);
	crystal1PitchEditor_->setTitle("Crystal A Pitch");

	crystal1RollEditor_ = new AMExtendedControlEditor(0);
	crystal1RollEditor_->setTitle("Crystal A Roll");

	crystal2PitchEditor_ = new AMExtendedControlEditor(0);
	crystal2PitchEditor_->setTitle("Crystal B Pitch");

	crystal2RollEditor_ = new AMExtendedControlEditor(0);
	crystal2RollEditor_->setTitle("Crystal B Roll");

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

	braggConfigWidget_ = new BioXASSSRLMonochromatorBraggConfigurationView(0);

	calibrateEnergyButton_ = new QPushButton("Calibrate Energy");
	calibrateGoniometerButton_ = new QPushButton("Calibrate Goniometer");

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Region");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	// Create and set layouts.
	// Motors view

	QVBoxLayout *motorsViewLayout = new QVBoxLayout();
	motorsViewLayout->addWidget(upperSlitEditor_);
	motorsViewLayout->addWidget(lowerSlitEditor_);
	motorsViewLayout->addWidget(heightEditor_);
	motorsViewLayout->addWidget(lateralEditor_);
	motorsViewLayout->addWidget(paddleEditor_);
	motorsViewLayout->addWidget(crystal1PitchEditor_);
	motorsViewLayout->addWidget(crystal1RollEditor_);
	motorsViewLayout->addWidget(crystal2PitchEditor_);
	motorsViewLayout->addWidget(crystal2RollEditor_);

	QGroupBox *motorsView = new QGroupBox("Motors");
	motorsView->setLayout(motorsViewLayout);
	motorsView->setMinimumWidth(VIEW_WIDTH_MIN);

	// Energy view

	QGridLayout *energyGridLayout = new QGridLayout();
	energyGridLayout->addWidget(stepEnergyEditor_, 0, 0);
	energyGridLayout->addWidget(encoderEnergyEditor_, 0, 1);
	energyGridLayout->addWidget(stepBraggEditor_, 1, 0);
	energyGridLayout->addWidget(encoderBraggEditor_, 1, 1);

	QHBoxLayout *energyM1Layout = new QHBoxLayout();
	energyM1Layout->addStretch();
	energyM1Layout->addWidget(m1PitchEditor_);
	energyM1Layout->addStretch();

	QVBoxLayout *energyViewLayout = new QVBoxLayout();
	energyViewLayout->addLayout(energyGridLayout);
	energyViewLayout->addLayout(energyM1Layout);

	QGroupBox *energyView = new QGroupBox("Energy");
	energyView->setLayout(energyViewLayout);
	energyView->setMinimumWidth(VIEW_WIDTH_MIN);

	// Bragg config view

	QVBoxLayout *braggConfigViewLayout = new QVBoxLayout();
	braggConfigViewLayout->setMargin(0);
	braggConfigViewLayout->addWidget(braggConfigWidget_);

	QGroupBox *braggConfigView = new QGroupBox("Goniometer configuration");
	braggConfigView->setLayout(braggConfigViewLayout);

	// Calibrate buttons view.

	QHBoxLayout *calibrateButtonsLayout = new QHBoxLayout();
	calibrateButtonsLayout->addWidget(calibrateEnergyButton_);
	calibrateButtonsLayout->addWidget(calibrateGoniometerButton_);

	QGroupBox *calibrateButtonsView = new QGroupBox("Calibration");
	calibrateButtonsView->setLayout(calibrateButtonsLayout);

	// Region view

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	QGroupBox *regionStatusView = new QGroupBox("Status");
	regionStatusView->setLayout(regionStatusViewLayout);

	QVBoxLayout *regionViewLayout = new QVBoxLayout();
	regionViewLayout->addWidget(regionEditor_);
	regionViewLayout->addWidget(regionStatusView);

	QGroupBox *regionView = new QGroupBox("Region");
	regionView->setLayout(regionViewLayout);
	regionView->setMinimumWidth(VIEW_WIDTH_MIN);

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
	rightLayout->addWidget(regionView);
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

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_) {

			// Clear UI elements.

			upperSlitEditor_->setControl(0);
			lowerSlitEditor_->setControl(0);
			heightEditor_->setControl(0);
			lateralEditor_->setControl(0);
			paddleEditor_->setControl(0);
			crystal1PitchEditor_->setControl(0);
			crystal1RollEditor_->setControl(0);
			crystal2PitchEditor_->setControl(0);
			crystal2RollEditor_->setControl(0);

			regionEditor_->setControl(0);
			regionStatusWidget_->setRegionControl(0);

			stepEnergyEditor_->setControl(0);
			encoderEnergyEditor_->setControl(0);
			stepBraggEditor_->setControl(0);
			encoderBraggEditor_->setControl(0);
			m1PitchEditor_->setControl(0);

			braggConfigWidget_->setBraggMotor(0);
		}

		mono_ = newMono;

		if (mono_) {

			// Update UI elements.

			upperSlitEditor_->setControl(mono_->upperSlitControl());
			lowerSlitEditor_->setControl(mono_->lowerSlitControl());
			heightEditor_->setControl(mono_->verticalMotor());
			lateralEditor_->setControl(mono_->lateralMotor());
			paddleEditor_->setControl(mono_->paddleControl());
			crystal1PitchEditor_->setControl(mono_->crystal1PitchMotor());
			crystal1RollEditor_->setControl(mono_->crystal1RollMotor());
			crystal2PitchEditor_->setControl(mono_->crystal2PitchMotor());
			crystal2RollEditor_->setControl(mono_->crystal2RollMotor());

			regionEditor_->setControl(mono_->regionControl());
			regionStatusWidget_->setRegionControl(mono_->regionControl());

			stepEnergyEditor_->setControl(mono_->stepEnergyControl());
			encoderEnergyEditor_->setControl(mono_->encoderEnergyControl());
			stepBraggEditor_->setControl(mono_->stepBraggControl());
			encoderBraggEditor_->setControl(mono_->encoderBraggControl());
			m1PitchEditor_->setControl(mono_->m1MirrorPitchControl());

			braggConfigWidget_->setBraggMotor(mono_->braggMotor());
		}

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onCalibrateEnergyButtonClicked()
{
	if (mono_) {
		AMControl *energyControl = mono_->energyControl();

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
		AMControl *braggMotor = mono_->braggMotor();

		if (braggMotor) {
			bool inputOK = false;
			double oldPosition = braggMotor->value();
			double newPosition = QInputDialog::getDouble(this, "Goniometer Calibration", "Enter calibrated goniometer position:", oldPosition, BRAGG_POSITION_MIN, BRAGG_POSITION_MAX, 2, &inputOK);

			if (inputOK)
				braggMotor->calibrate(oldPosition, newPosition);
		}
	}
}









BioXASSSRLMonochromatorBraggConfigurationView::BioXASSSRLMonochromatorBraggConfigurationView(CLSMAXvMotor *braggMotor, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	braggMotor_ = 0;

	// Create UI elements.

	QLabel *braggVelocityPrompt = new QLabel("Velocity:");
	braggVelocity_ = new QDoubleSpinBox();

	QLabel *braggVelocityBasePrompt = new QLabel("Velocity base:");
	braggVelocityBase_ = new QDoubleSpinBox();

	QLabel *braggAccelerationPrompt = new QLabel("Acceleration:");
	braggAcceleration_ = new QDoubleSpinBox();

	QLabel *braggEncoderMoveTypePrompt = new QLabel("Encoder move type:");
	braggEncoderMoveType_ = new QComboBox();

	QLabel *braggSettlingTimePrompt = new QLabel("Settling time:");
	braggSettlingTime_ = new QDoubleSpinBox();

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(braggVelocityPrompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggVelocity_, 0, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(braggVelocityBasePrompt, 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggVelocityBase_, 1, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(braggAccelerationPrompt, 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggAcceleration_, 2, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(braggEncoderMoveTypePrompt, 3, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggEncoderMoveType_, 3, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(braggSettlingTimePrompt, 4, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggSettlingTime_, 4, 1, 1, 1, Qt::AlignLeft);

	setLayout(layout);

	// Initial settings.

	clearUI();

	// Current settings.

	setBraggMotor(braggMotor);
}

BioXASSSRLMonochromatorBraggConfigurationView::~BioXASSSRLMonochromatorBraggConfigurationView()
{

}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotor(CLSMAXvMotor *newMotor)
{
	if (braggMotor_ != newMotor) {

		if (braggMotor_) {
			disconnect( braggMotor_, 0, this, 0 );
		}

		braggMotor_ = newMotor;

		if (braggMotor_) {
			connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onBraggMotorConnectedChanged()) );
			connect( braggMotor_, SIGNAL(EGUVelocityChanged(double)), this, SLOT(onBraggMotorVelocityChanged()) );
			connect( braggMotor_, SIGNAL(EGUBaseVelocityChanged(double)), this, SLOT(onBraggMotorVelocityBaseChanged()) );
			connect( braggMotor_, SIGNAL(EGUAccelerationChanged(double)), this, SLOT(onBraggMotorAccelerationChanged()) );
			connect( braggMotor_, SIGNAL(encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType)), this, SLOT(onBraggMotorEncoderMoveTypeChanged()) );
			connect( braggMotor_, SIGNAL(settlingTimeChanged(double)), this, SLOT(onBraggMotorSettlingTimeChanged()) );
		}

		onBraggMotorConnectedChanged();

		emit braggMotorChanged(braggMotor_);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::clearUI()
{
	braggVelocity_->disconnect();
	braggVelocity_->clear();
	braggVelocity_->setEnabled(false);

	braggVelocityBase_->disconnect();
	braggVelocityBase_->clear();
	braggVelocityBase_->setEnabled(false);

	braggAcceleration_->disconnect();
	braggAcceleration_->clear();
	braggAcceleration_->setEnabled(false);

	braggEncoderMoveType_->disconnect();
	braggEncoderMoveType_->clear();
	braggEncoderMoveType_->setEnabled(false);

	braggSettlingTime_->disconnect();
	braggSettlingTime_->clear();
	braggSettlingTime_->setEnabled(false);
}

void BioXASSSRLMonochromatorBraggConfigurationView::initializeUI()
{
	if (braggMotor_ && braggMotor_->isConnected()) {

		braggVelocity_->setEnabled(true);
		braggVelocity_->setMinimum(BRAGG_VELOCITY_MIN);
		braggVelocity_->setMaximum(BRAGG_VELOCITY_MAX);
		braggVelocity_->setValue(braggMotor_->EGUVelocity());
		braggVelocity_->setSuffix(" deg/s");

		braggVelocityBase_->setEnabled(true);
		braggVelocityBase_->setMinimum(BRAGG_BASE_VELOCITY_MIN);
		braggVelocityBase_->setMaximum(BRAGG_BASE_VELOCITY_MAX);
		braggVelocityBase_->setValue(braggMotor_->EGUBaseVelocity());
		braggVelocityBase_->setSuffix(" deg/s");

		braggAcceleration_->setEnabled(true);
		braggAcceleration_->setMinimum(BRAGG_ACCELERATION_MIN);
		braggAcceleration_->setMaximum(BRAGG_ACCELERATION_MAX);
		braggAcceleration_->setValue(braggMotor_->EGUAcceleration());
		braggAcceleration_->setSuffix(" deg/s/s");

		braggEncoderMoveType_->setEnabled(true);
		braggEncoderMoveType_->addItem("Off");
		braggEncoderMoveType_->addItem("Increase Only");
		braggEncoderMoveType_->addItem("Decrease Only");
		braggEncoderMoveType_->addItem("Increase");
		braggEncoderMoveType_->addItem("Decrease");
		braggEncoderMoveType_->setCurrentIndex(braggMotor_->encoderMovementType());

		braggSettlingTime_->setEnabled(true);
		braggSettlingTime_->setMinimum(BRAGG_SETTLING_TIME_MIN);
		braggSettlingTime_->setMaximum(BRAGG_SETTLING_TIME_MAX);
		braggSettlingTime_->setValue(braggMotor_->settlingTime());
		braggSettlingTime_->setSuffix("s");

		connect( braggVelocity_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorVelocity(double)) );
		connect( braggVelocityBase_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorVelocityBase(double)) );
		connect( braggAcceleration_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorAcceleration(double)) );
		connect( braggEncoderMoveType_, SIGNAL(currentIndexChanged(int)), this, SLOT(setBraggMotorEncoderMovementType(int)) );
		connect( braggSettlingTime_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorSettlingTime(double)) );
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::refreshUI()
{
	clearUI();
	initializeUI();
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorConnectedChanged()
{
	refreshUI();
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorVelocityChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggVelocity_->setValue(braggMotor_->EGUVelocity());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorVelocityBaseChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggVelocityBase_->setValue(braggMotor_->EGUBaseVelocity());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorAccelerationChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggAcceleration_->setValue(braggMotor_->EGUAcceleration());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorEncoderMoveTypeChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggEncoderMoveType_->setCurrentIndex(braggMotor_->encoderMovementType());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorSettlingTimeChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggSettlingTime_->setValue(braggMotor_->settlingTime());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorVelocity(double newVelocity)
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggMotor_->setEGUVelocity(newVelocity);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorVelocityBase(double newBase)
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggMotor_->setEGUBaseVelocity(newBase);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorAcceleration(double newAcceleration)
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggMotor_->setEGUAcceleration(newAcceleration);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorEncoderMovementType(int newType)
{
	if (braggMotor_) {
		switch (newType) {
		case CLSMAXvMotor::EncoderMovementOff:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementOff);
			break;
		case CLSMAXvMotor::EncoderMovementIncreaseOnly:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementIncreaseOnly);
			break;
		case CLSMAXvMotor::EncoderMovementDecreaseOnly:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementDecreaseOnly);
			break;
		case CLSMAXvMotor::EncoderMovementIncrease:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementIncrease);
			break;
		case CLSMAXvMotor::EncoderMovementDecrease:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementDecrease);
			break;
		default:
			break;
		}
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorSettlingTime(double seconds)
{
	if (braggMotor_) {
		braggMotor_->setSettlingTime(seconds);
	}
}
