#include "BioXASMainMonochromator.h"
#include <QDebug>

BioXASMainMonochromator::BioXASMainMonochromator(QObject *parent) :
	BioXASSSRLMonochromator(parent)
{
    // Initialize variables.

    connected_ = false;

	// Create components.

	upperSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	lowerSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	paddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), false, 0.1, 2.0, this);
    braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
    verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this);
    lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
    crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
    crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// Create controls.

	region_ = new BioXASMainMonochromatorRegionControl(this);
    energy_ = new BioXASMainMonochromatorControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString("BL1607-5-I21:Energy:stop"), this);
	slitsStatus_ = new AMReadOnlyPVControl(QString("SlitsStatus"), QString("BL1607-5-I21:Mono:SlitsClosed"), this);
	paddleStatus_ = new AMReadOnlyPVControl(QString("PaddleStatus"), QString("BL1607-5-I21:Mono:PaddleExtracted"), this);
	keyStatus_ = new AMReadOnlyPVControl(QString("KeyStatus"), QString("BL1607-5-I21:Mono:KeyStatus"), this);
	brakeStatus_ = new AMReadOnlyPVControl(QString("BrakeStatus"), QString("BL1607-5-I21:Mono:BrakeOff"), this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this);

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

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}

AMAction3* BioXASMainMonochromator::createSetRegionAction(double newRegion)
{
	if (!region_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(region_, newRegion);
}

AMAction3* BioXASMainMonochromator::createSetEnergyAction(double newEnergy)
{
    if (!energy_->isConnected())
        return 0;

	return AMActionSupport::buildControlMoveAction(energy_, newEnergy);
}

void BioXASMainMonochromator::setRegion(double newRegion)
{
	if (region_->isConnected())
		region_->move(newRegion);
}

void BioXASMainMonochromator::setEnergy(double newEnergy)
{
	if (energy_->isConnected())
		energy_->move(newEnergy);
}

void BioXASMainMonochromator::onConnectedChanged()
{
    bool currentState = (

				region_->isConnected() &&
				energy_->isConnected() &&
				keyStatus_->isConnected() &&
				brakeStatus_->isConnected() &&
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

