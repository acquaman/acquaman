#include "BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideM2MirrorBendControl.h"

BioXASSideM2Mirror::BioXASSideM2Mirror(QObject *parent) :
	BioXASM2Mirror("SideM2Mirror", parent)
{
	// Initialize member variables.

	upstreamLength_ = -543.725;
	downstreamLength_ = 543.725;

	upstreamInboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), true, -619.125, 61.934, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), true, -619.125, -141.266, 0.05, 2.0, this, QString(":mm"));
	downstreamMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelectMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yawMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-19 YAW"), QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	screenMotor_ = new AMSinglePVControl(name_+"FluorescentScreen", "VSC1607-5-I22-02:InBeam", this);

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

	yaw_ = new BioXASMirrorYawControl(name_+"YawControl", "deg", upstreamLength_, downstreamLength_, this);
	yaw_->setYawControl(yawMotor_);

	bend_ = new BioXASSideM2MirrorBendControl(name_+"BendControl", "m", upstreamLength_, downstreamLength_, this);
	bend_->setUpstreamBenderControl(benderUpstreamMotor_);
	bend_->setDownstreamBenderControl(benderDownstreamMotor_);

	// Make connections.

	connect( upstreamInboardMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( upstreamOutboardMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( downstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelectMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yawMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstreamMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( screenMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( pitch_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( roll_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( height_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( bend_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
