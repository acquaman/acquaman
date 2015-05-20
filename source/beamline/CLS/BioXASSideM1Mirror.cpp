#include "BioXASSideM1Mirror.h"

BioXASSideM1Mirror::BioXASSideM1Mirror(QObject *parent) :
    BioXASM1Mirror(parent)
{
	// Initialize inherited variables.

	verticalUpstreamInb_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalUpstreamOutb_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));

	// Make connections.

	connect( verticalUpstreamInb_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalUpstreamOutb_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stripeSelect_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( yaw_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( benderDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASSideM1Mirror::~BioXASSideM1Mirror()
{

}
