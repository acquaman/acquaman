#include "BioXASMainM1Mirror.h"

BioXASMainM1Mirror::BioXASMainM1Mirror(QObject *parent) :
	BioXASM1Mirror("MainM1Mirror", parent)
{
	// Initialize inherited variables.

	upstreamLength_ = -543.725;
	downstreamLength_ = 543.725;

	upstreamInboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-01"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), true, -619.125, 190.438, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-02"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), true, -619.125, -12.763, 0.05, 2.0, this, QString(":mm"));
	downstreamMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-03"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelectMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-04 STRIPE SELECT"), QString("SMTR1607-5-I21-04"), QString("SMTR1607-5-I21-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yawMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-05 YAW"), QString("SMTR1607-5-I21-05"), QString("SMTR1607-5-I21-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-06"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-07"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	upperSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-08 UPPER SLIT"), QString("SMTR1607-5-I21-08"), QString("SMTR1607-5-I21-08 UPPER SLIT"), true, 0.05, 2.0, this, QString(":mm"));

	pitch_ = new BioXASMirrorPitchControl(name_+"PitchControl", "deg", this);
	pitch_->setUpstreamInboardMotor(upstreamInboardMotor_);
	pitch_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
	pitch_->setDownstreamMotor(downstreamMotor_);

	roll_ = new BioXASMirrorRollControl(name_+"RollControl", "deg", this);
	roll_->setUpstreamInboardMotor(upstreamInboardMotor_);
	roll_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
	roll_->setDownstreamMotor(downstreamMotor_);

	height_ = new BioXASMirrorHeightControl(name_+"HeightControl", "mm", this);
	height_->setUpstreamInboardMotor(upstreamInboardMotor_);
	height_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
	height_->setDownstreamMotor(downstreamMotor_);

	lateral_ = new BioXASMirrorLateralControl(name_+"LateralControl", "mm", this);
	lateral_->setStripeSelectionMotor(stripeSelectMotor_);
	lateral_->setYawMotor(yawMotor_);
	lateral_->setUpstreamLength(upstreamLength_);
	lateral_->setDownstreamLength(downstreamLength_);

	yaw_ = new BioXASMirrorYawControl(name_+"YawControl", "deg", this);
	yaw_->setYawMotor(yawMotor_);
	yaw_->setUpstreamLength(upstreamLength_);
	yaw_->setDownstreamLength(downstreamLength_);

	// Make connections.

	connect( upstreamInboardMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upstreamOutboardMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( downstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelectMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yawMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upperSlitBladeMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( pitch_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( roll_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( height_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASMainM1Mirror::~BioXASMainM1Mirror()
{

}
