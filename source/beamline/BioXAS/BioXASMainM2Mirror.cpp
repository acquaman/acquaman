#include "BioXASMainM2Mirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"

BioXASMainM2Mirror::BioXASMainM2Mirror(QObject *parent) :
	BioXASM2Mirror("MainM2Mirror", parent)
{
	// Initialize member variables.

	upstreamInboard_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	upstreamOutboard_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	downstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	screen_ = new AMSinglePVControl("M2FluorescenceScreen", "VSC1607-5-I21-02:InBeam", this);

	pitch_ = new BioXASMirrorPitchControl("M2PitchControl", "deg", this);
	pitch_->setUpstreamInboardControl(upstreamInboard_);
	pitch_->setUpstreamOutboardControl(upstreamOutboard_);
	pitch_->setDownstreamControl(downstream_);

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
}

BioXASMainM2Mirror::~BioXASMainM2Mirror()
{

}
