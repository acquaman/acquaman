#include "BioXASMainM2Mirror.h"
#include "beamline/BioXAS/BioXASMainM2MirrorBendControl.h"

BioXASMainM2Mirror::BioXASMainM2Mirror(QObject *parent) :
	BioXASM2Mirror("MainM2Mirror", parent)
{
	// Initialize member variables.

	upstreamLength_ = -543.77;
	downstreamLength_ = 543.68;

	upstreamInboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, -619.125, 61.933, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboardMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, -619.125, -141.268, 0.05, 2.0, this, QString(":mm"));
	downstreamMotor_ = new BioXASMirrorMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm"));
	stripeSelectMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yawMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.1, 2.0, this, QString(":lbs"));
	benderDownstreamMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.1, 2.0, this, QString(":lbs"));
	screenMotor_ = new AMSinglePVControl("M2FluorescentScreen", "VSC1607-5-I21-02:InBeam", this);

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
	lateral_->setUpstreamLength(upstreamLength_);
	lateral_->setDownstreamLength(downstreamLength_);
	lateral_->setStripeSelectionMotor(stripeSelectMotor_);
	lateral_->setYawMotor(yawMotor_);

	yaw_ = new BioXASMirrorYawControl(name_+"YawControl", "deg", this);
	yaw_->setUpstreamLength(upstreamLength_);
	yaw_->setDownstreamLength(downstreamLength_);
	yaw_->setYawMotor(yawMotor_);
	yaw_->setStripeSelectionMotor(stripeSelectMotor_);

	bend_ = new BioXASMainM2MirrorBendControl(name_+"BendControl", "m", this);
	bend_->setUpstreamBenderMotor(benderUpstreamMotor_);
	bend_->setDownstreamBenderMotor(benderDownstreamMotor_);

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

BioXASMainM2Mirror::~BioXASMainM2Mirror()
{

}
