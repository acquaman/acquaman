#include "BioXASSideM1Mirror.h"

BioXASSideM1Mirror::BioXASSideM1Mirror(QObject *parent) :
    BioXASM1Mirror(parent)
{
	// Initialize inherited variables.

	verticalUpstreamInb_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalUpstreamOutb_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	verticalDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":mm"));
	stripeSelect_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm"));
	yaw_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-05 YAW"), QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"), true, 0.05, 2.0, this, QString(":mm"));
	benderUpstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	benderDownstream_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), true, 0.05, 2.0, this, QString(":lbs"));
	upperSlitBlade_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-08 UPPER SLIT"), QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT"), true, 0.05, 2.0, this, QString(":mm"));

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
