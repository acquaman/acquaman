#include "BioXASSSRLMonochromatorConfigurationView.h"
#include <QDebug>

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = mono;

	regionEditor_ = 0;
	energyEditor_ = 0;
	calibrateEnergyButton_ = 0;
	braggEditor_ = 0;
	calibrateBraggButton_ = 0;

	regionStatusWidget_ = 0;
	braggConfigWidget_ = 0;

	if (mono_) {

		// Create UI elements.

		regionEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(mono_->regionControl());
		regionEditor_->setTitle("Region");

		energyEditor_ = new AMExtendedControlEditor(mono_->energyControl());
		energyEditor_->setTitle("Energy EV");
		energyEditor_->setControlFormat('f', 2);

		calibrateEnergyButton_ = new QPushButton("Calibrate");

		braggEditor_ = new AMExtendedControlEditor(mono_->braggMotor());
		braggEditor_->setTitle("Bragg motor position");

		calibrateBraggButton_ = new QPushButton("Calibrate");

		QGroupBox *regionStatusView = new QGroupBox("Region status");

		regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(mono_->regionControl());

		QGroupBox *braggConfigView = new QGroupBox("Bragg configuration");

		braggConfigWidget_ = new BioXASSSRLMonochromatorBraggConfigurationView(mono_->braggMotor());

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

		regionStatusView->setLayout(regionStatusViewLayout);

		QVBoxLayout *braggConfigViewLayout = new QVBoxLayout();
		braggConfigViewLayout->setMargin(0);
		braggConfigViewLayout->addWidget(braggConfigWidget_);

		braggConfigView->setLayout(braggConfigViewLayout);

		QVBoxLayout *layoutV = new QVBoxLayout();
		layoutV->addWidget(regionEditor_);
		layoutV->addLayout(energyLayout);
		layoutV->addLayout(braggLayout);
		layoutV->addWidget(regionStatusView);
		layoutV->addWidget(braggConfigView);
		layoutV->addStretch();

		QHBoxLayout *layoutH = new QHBoxLayout();
		layoutH->addLayout(layoutV);
		layoutH->addStretch();

		setLayout(layoutH);

		// Make connections

		connect( calibrateEnergyButton_, SIGNAL(clicked()), this, SLOT(onCalibrateEnergyButtonClicked()) );
		connect( calibrateBraggButton_, SIGNAL(clicked()), this, SLOT(onCalibrateBraggButtonClicked()) );
	}
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::onCalibrateEnergyButtonClicked()
{
	if (mono_) {
		bool inputOK = false;
		double newEnergy = QInputDialog::getDouble(this, "Energy Calibration", "Enter calibrated energy:", mono_->energyControl()->value(), ENERGY_MIN, ENERGY_MAX, 1, &inputOK);

		if (inputOK) {
			mono_->energyControl()->setEnergyCalibration(newEnergy);
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

	QLabel *braggEncoderMoveTypePrompt = new QLabel("Encoder move type:");

	braggEncoderMoveType_ = new QComboBox();

	QLabel *braggSettlingTimePrompt = new QLabel("Settling time:");

	braggSettlingTime_ = new QDoubleSpinBox();

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(braggEncoderMoveTypePrompt, 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggEncoderMoveType_, 1, 1, 1, 1, Qt::AlignCenter);
	layout->addWidget(braggSettlingTimePrompt, 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggSettlingTime_, 2, 1, 1, 1, Qt::AlignCenter);

	setLayout(layout);

	// Initial settings.

	clearUI();

	// Make connections.

	connect( braggEncoderMoveType_, SIGNAL(currentIndexChanged(int)), this, SLOT(setBraggMotorEncoderMovementType(int)) );
	connect( braggSettlingTime_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorSettlingTime(double)) );

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
			connect( braggMotor_, SIGNAL(encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType)), this, SLOT(onBraggMotorEncoderMoveTypeChanged()) );
			connect( braggMotor_, SIGNAL(settlingTimeChanged(double)), this, SLOT(onBraggMotorSettlingTimeChanged()) );
		}

		onBraggMotorConnectedChanged();

		emit braggMotorChanged(braggMotor_);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::clearUI()
{
	braggEncoderMoveType_->clear();
	braggEncoderMoveType_->setEnabled(false);

	braggSettlingTime_->clear();
	braggSettlingTime_->setEnabled(false);
}

void BioXASSSRLMonochromatorBraggConfigurationView::initializeUI()
{
	if (braggMotor_ && braggMotor_->isConnected()) {

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

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorSettlingTime(double newTime)
{
	if (braggMotor_) {
		braggMotor_->setSettlingTime(newTime);
	}
}
