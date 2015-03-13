#include "BioXASSideMonochromator.h"

#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator(parent)
{
	// Initialize variables.

	connected_ = false;

	// Create components.

	upperSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	lowerSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	paddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this);
	braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this);
	lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// Create controls.

	region_ = new BioXASSideMonochromatorRegionControl(this);
	energy_ = new BioXASSideMonochromatorControl("EnergyEV", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", QString("BL1607-5-I22:Energy:stop"), this);

	// Listen to value changes.

	connect( region_, SIGNAL(valueChanged(double)), this, SIGNAL(regionChanged(double)) );
	connect( energy_, SIGNAL(valueChanged(double)), this, SIGNAL(energyChanged(double)) );
	connect( slitsStatus_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsStatusChanged(double)) );
	connect( paddleStatus_, SIGNAL(valueChanged(double)), this, SLOT(onPaddleStatusChanged(double)) );
	connect( keyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged(double)) );
	connect( brakeStatus_, SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged(double)) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(valueChanged(double)), SLOT(onBraggAtCrystalChangePositionStatusChanged(double)) );

	// Listen to connection states.

	connect( region_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( slitsStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( paddleStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( brakeStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( hc_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal2D_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggAngle_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( upperSlitMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( lowerSlitMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( paddleMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( verticalMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( lateralMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystalChangeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal1PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal1RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal2PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal2RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	// Current settings.

	onConnectedChanged();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

void BioXASSideMonochromator::onConnectedChanged()
{
	bool currentState = (

		region_->isConnected() &&
		energy_->isConnected() &&
		slitsStatus_->isConnected() &&
		paddleStatus_->isConnected() &&
		keyStatus_->isConnected() &&
		brakeStatus_->isConnected() &&
		braggAtCrystalChangePositionStatus_->isConnected() &&
		hc_->isConnected() &&
		crystal2D_->isConnected() &&
		braggAngle_->isConnected() &&

		upperSlitMotor_->isConnected() &&
		lowerSlitMotor_->isConnected() &&
		paddleMotor_->isConnected() &&
		braggMotor_->isConnected() &&
		verticalMotor_->isConnected() &&
		lateralMotor_->isConnected() &&
		crystalChangeMotor_->isConnected() &&
		crystal1PitchMotor_->isConnected() &&
		crystal1RollMotor_->isConnected() &&
		crystal2PitchMotor_->isConnected() &&
		crystal2RollMotor_->isConnected()

	);

	if (connected_ != currentState) {
		connected_ = currentState;
		emit connected(connected_);
	}
}
