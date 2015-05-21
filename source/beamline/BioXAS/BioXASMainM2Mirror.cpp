#include "BioXASMainM2Mirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"

BioXASMainM2Mirror::BioXASMainM2Mirror(QObject *parent) :
    BioXASM2Mirror(parent)
{
	// Initialize member variables.

	verticalUpstreamInb_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalUpstreamOutb_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	screen_ = new AMSinglePVControl("M2FluorescenceScreen", "VSC1607-5-I21-02:InBeam", this);

//	pseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Roll", "BL1607-5-I21:M2:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
//	pseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Pitch", "BL1607-5-I21:M2:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
//	pseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Height", "BL1607-5-I21:M2:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
//	pseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Yaw", "BL1607-5-I21:M2:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
//	pseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I21 Main M2 Lateral", "BL1607-5-I21:M2:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// Make connections.

	connect( verticalUpstreamInb_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalUpstreamOutb_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelect_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( screen_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

//	connect( pseudoRoll_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
//	connect( pseudoPitch_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
//	connect( pseudoYaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
//	connect( pseudoHeight_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
//	connect( pseudoLateral_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASMainM2Mirror::~BioXASMainM2Mirror()
{

}
