#include "BioXASSSRLMonochromatorConfigurationView.h"
#include <QDebug>

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(0);
	regionEditor_->setTitle("Region");

	energyEditor_ = new AMExtendedControlEditor(0);
	energyEditor_->setTitle("Energy");
	energyEditor_->setControlFormat('f', 2);

	calibrateEnergyButton_ = new QPushButton("Calibrate");

	braggEditor_ = new AMExtendedControlEditor(0);
	braggEditor_->setTitle("Goniometer angle");

	calibrateBraggButton_ = new QPushButton("Calibrate");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	braggConfigWidget_ = new BioXASSSRLMonochromatorBraggConfigurationView(0);

	// Create and set layouts.

	QHBoxLayout *energyLayout = new QHBoxLayout();
	energyLayout->setMargin(0);
	energyLayout->addWidget(energyEditor_);
	energyLayout->addWidget(calibrateEnergyButton_);

	QHBoxLayout *braggLayout = new QHBoxLayout();
	braggLayout->setMargin(0);
	braggLayout->addWidget(braggEditor_);
	braggLayout->addWidget(calibrateBraggButton_);

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	QGroupBox *regionStatusView = new QGroupBox("Region status");
	regionStatusView->setLayout(regionStatusViewLayout);

	QVBoxLayout *braggConfigViewLayout = new QVBoxLayout();
	braggConfigViewLayout->setMargin(0);
	braggConfigViewLayout->addWidget(braggConfigWidget_);

	QGroupBox *braggConfigView = new QGroupBox("Bragg configuration");
	braggConfigView->setLayout(braggConfigViewLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(regionEditor_);
	layout->addLayout(energyLayout);
	layout->addLayout(braggLayout);
	layout->addWidget(regionStatusView);
	layout->addWidget(braggConfigView);
	layout->addStretch();

	setLayout(layout);

	// Make connections

	connect( calibrateEnergyButton_, SIGNAL(clicked()), this, SLOT(onCalibrateEnergyButtonClicked()) );
	connect( calibrateBraggButton_, SIGNAL(clicked()), this, SLOT(onCalibrateBraggButtonClicked()) );

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

			regionEditor_->setControl(0);
			energyEditor_->setControl(0);
			braggEditor_->setControl(0);

			regionStatusWidget_->setRegionControl(0);
			braggConfigWidget_->setBraggMotor(0);
		}

		mono_ = newMono;

		if (mono_) {

			// Update UI elements.

			regionEditor_->setControl(mono_->regionControl());
			energyEditor_->setControl(mono_->energyControl());
			braggEditor_->setControl(mono_->braggMotor());

			regionStatusWidget_->setRegionControl(mono_->regionControl());
			braggConfigWidget_->setBraggMotor(mono_->braggMotor());
		}

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onCalibrateEnergyButtonClicked()
{
	if (mono_) {
		bool inputOK = false;
		double newEnergy = QInputDialog::getDouble(this, "Energy Calibration", "Enter calibrated energy:", mono_->energyControl()->value(), ENERGY_MIN, ENERGY_MAX, 1, &inputOK);

		if (inputOK) {
			mono_->energyControl()->setEnergy(newEnergy);
		}
	}
}

void BioXASSSRLMonochromatorConfigurationView::onCalibrateBraggButtonClicked()
{
	if (mono_) {
		bool inputOK = false;
		double newPosition = QInputDialog::getDouble(this, "Bragg Position Calibration", "Enter calibrated position:", mono_->braggMotor()->value(), BRAGG_POSITION_MIN, BRAGG_POSITION_MAX, 1, &inputOK);

		if (inputOK) {
			mono_->calibrateBraggPosition(newPosition);
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
