#include "BioXASSideM1Mirror.h"

BioXASSideM1Mirror::BioXASSideM1Mirror(QObject *parent) :
	BioXASM1Mirror("SideM1Mirror", parent)
{
	// Initialize inherited variables.

	upstreamLength_ = -543.725;
	downstreamLength_ = 543.725;

	upstreamInboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), true, -619.125, 190.438, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), true, -619.125, -12.763, 0.05, 2.0, this, QString(":mm"));
	downstreamMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelectMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yawMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-05 YAW"), QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	upperSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-08 UPPER SLIT"), QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT"), true, 0.05, 2.0, this, QString(":mm"));

	pitch_ = new BioXASMirrorPitchControl(name_+"PitchControl", "deg", upstreamLength_, downstreamLength_, this);
	pitch_->setUpstreamInboardControl(upstreamInboardMotor_);
	pitch_->setUpstreamOutboardControl(upstreamOutboardMotor_);
	pitch_->setDownstreamControl(downstreamMotor_);

	roll_ = new BioXASMirrorRollControl(name_+"RollControl", "deg", upstreamLength_, downstreamLength_, this);
	roll_->setUpstreamInboardControl(upstreamInboardMotor_);
	roll_->setUpstreamOutboardControl(upstreamOutboardMotor_);
	roll_->setDownstreamControl(downstreamMotor_);

	height_ = new BioXASMirrorHeightControl(name_+"HeightControl", "mm", upstreamLength_, downstreamLength_, this);
	height_->setUpstreamInboardControl(upstreamInboardMotor_);
	height_->setUpstreamOutboardControl(upstreamOutboardMotor_);
	height_->setDownstreamControl(downstreamMotor_);

	lateral_ = new BioXASMirrorLateralControl(name_+"LateralControl", "mm", upstreamLength_, downstreamLength_, this);
	lateral_->setStripeSelectionControl(stripeSelectMotor_);
	lateral_->setYawControl(yawMotor_);
	lateral_->setUpstreamLength(upstreamLength_);
	lateral_->setDownstreamLength(downstreamLength_);

	yaw_ = new BioXASMirrorYawControl(name_+"YawControl", "deg", upstreamLength_, downstreamLength_, this);
	yaw_->setYawControl(yawMotor_);

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
}

BioXASSideM1Mirror::~BioXASSideM1Mirror()
{

}
