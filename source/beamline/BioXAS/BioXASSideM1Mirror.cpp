#include "BioXASSideM1Mirror.h"

BioXASSideM1Mirror::BioXASSideM1Mirror(QObject *parent) :
	BioXASM1Mirror("SideM1Mirror", parent)
{
	// Initialize inherited variables.

	upstreamInboard_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), true, -619.125, 190.438, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboard_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), true, -619.125, -12.763, 0.05, 2.0, this, QString(":mm"));
	downstream_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-05 YAW"), QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	upperSlitBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-08 UPPER SLIT"), QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT"), true, 0.05, 2.0, this, QString(":mm"));

	pitch_ = new BioXASMirrorPitchControl(name_+"PitchControl", "deg", this);
	pitch_->setUpstreamInboardControl(upstreamInboard_);
	pitch_->setUpstreamOutboardControl(upstreamOutboard_);
	pitch_->setDownstreamControl(downstream_);

	roll_ = new BioXASMirrorRollControl(name_+"RollControl", "deg", this);
	roll_->setUpstreamInboardControl(upstreamInboard_);
	roll_->setUpstreamOutboardControl(upstreamOutboard_);
	roll_->setDownstreamControl(downstream_);

	height_ = new BioXASMirrorHeightControl(name_+"HeightControl", "mm", this);
	height_->setUpstreamInboardControl(upstreamInboard_);
	height_->setUpstreamOutboardControl(upstreamOutboard_);
	height_->setDownstreamControl(downstream_);

	lateral_ = new BioXASMirrorLateralControl(name_+"LateralControl", "mm", this);
	lateral_->setStripeSelectionControl(stripeSelect_);
	lateral_->setYawControl(yaw_);
	lateral_->setUpstreamLength(-543.725);
	lateral_->setDownstreamLength(543.725);

	// Make connections.

	connect( upstreamInboard_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upstreamOutboard_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( downstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelect_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upperSlitBlade_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( pitch_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( roll_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( height_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASSideM1Mirror::~BioXASSideM1Mirror()
{

}
