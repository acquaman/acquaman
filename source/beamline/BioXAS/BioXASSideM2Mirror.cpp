#include "BioXASSideM2Mirror.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"

BioXASSideM2Mirror::BioXASSideM2Mirror(QObject *parent) :
    BioXASM2Mirror(parent)
{
	// Initialize member variables.

	screen_ = new AMSinglePVControl("M2FluorescenceScreen", "VSC1607-5-I22-02:InBeam", this);
	verticalUpstreamInb_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalUpstreamOutb_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-19 YAW"), QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));

	pseudoRoll_ = new BioXASPseudoMotorControl("BL1607-5-I22 Side M2 Roll", "BL1607-5-I22:M2:Roll", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	pseudoPitch_ = new BioXASPseudoMotorControl("BL1607-5-I22 Side M2 Pitch", "BL1607-5-I22:M2:Pitch", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	pseudoYaw_ = new BioXASPseudoMotorControl("BL1607-5-I22 Side M2 Yaw", "BL1607-5-I22:M2:Yaw", ":deg:fbk", ":deg", ":status", ":enabled", ":stop");
	pseudoHeight_ = new BioXASPseudoMotorControl("BL1607-5-I22 Side M2 Height", "BL1607-5-I22:M2:Height", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");
	pseudoLateral_ = new BioXASPseudoMotorControl("BL1607-5-I22 Side M2 Lateral", "BL1607-5-I22:M2:Lateral", ":mm:fbk", ":mm", ":status", ":enabled", ":stop");

	// Make connections.

	connect( screen_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( verticalUpstreamInb_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( verticalUpstreamOutb_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( verticalDownstream_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( stripeSelect_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( benderUpstream_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( benderDownstream_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	connect( pseudoRoll_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( pseudoPitch_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( pseudoYaw_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( pseudoHeight_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( pseudoLateral_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
