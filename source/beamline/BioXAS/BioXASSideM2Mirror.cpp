#include "BioXASSideM2Mirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"

BioXASSideM2Mirror::BioXASSideM2Mirror(QObject *parent) :
	BioXASM2Mirror("SideM2Mirror", parent)
{
	// Initialize member variables.

	upstreamLength_ = -543.725;
	downstreamLength_ = 543.725;

	upstreamInboard_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), true, -619.125, 61.934, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboard_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), true, -619.125, -141.266, 0.05, 2.0, this, QString(":mm"));
	downstream_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-19 YAW"), QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	screen_ = new AMSinglePVControl(name_+"FluorescentScreen", "VSC1607-5-I22-02:InBeam", this);

	pitch_ = new BioXASMirrorPitchControl(name_+"PitchControl", "deg", upstreamLength_, downstreamLength_, this);
	pitch_->setUpstreamInboardControl(upstreamInboard_);
	pitch_->setUpstreamOutboardControl(upstreamOutboard_);
	pitch_->setDownstreamControl(downstream_);

	roll_ = new BioXASMirrorRollControl(name_+"RollControl", "deg", upstreamLength_, downstreamLength_, this);
	roll_->setUpstreamInboardControl(upstreamInboard_);
	roll_->setUpstreamOutboardControl(upstreamOutboard_);
	roll_->setDownstreamControl(downstream_);

	height_ = new BioXASMirrorHeightControl(name_+"HeightControl", "mm", upstreamLength_, downstreamLength_, this);
	height_->setUpstreamInboardControl(upstreamInboard_);
	height_->setUpstreamOutboardControl(upstreamOutboard_);
	height_->setDownstreamControl(downstream_);

	lateral_ = new BioXASMirrorLateralControl(name_+"LateralControl", "mm", upstreamLength_, downstreamLength_, this);
	lateral_->setStripeSelectionControl(stripeSelect_);
	lateral_->setYawControl(yaw_);

	// Make connections.

	connect( upstreamInboard_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upstreamOutboard_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( downstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelect_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( screen_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( pitch_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( roll_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( height_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
