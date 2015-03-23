#include "BioXASMainMonochromator.h"

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

	hc_ = new AMReadOnlyPVControl(QString("hcConstant"), QString("BL1607-5-I21:Energy:EV:fbk:tr.A"), this);
	crystal2D_ = new AMReadOnlyPVControl(QString("Crystal2DSpacing"), QString("BL1607-5-I21:Energy:EV:fbk:tr.B"), this);
	braggAngle_ = new AMReadOnlyPVControl(QString("BraggAngle"), QString("BL1607-5-I21:Energy:EV:fbk:tr.K"), this);

	region_ = new BioXASMainMonochromatorRegionControl(this);
	energy_ = new BioXASMainMonochromatorEnergyControl(this);


	// Listen to value changes.

	connect( region_, SIGNAL(valueChanged(double)), this, SIGNAL(regionChanged(double)) );
	connect( energy_, SIGNAL(valueChanged(double)), this, SIGNAL(energyChanged(double)) );

	// Listen to connection states.

	connect( region_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
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

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}

void BioXASMainMonochromator::onConnectedChanged()
{
	bool currentState = (

		region_->isConnected() &&
		energy_->isConnected() &&
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

