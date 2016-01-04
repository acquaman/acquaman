#include "BioXASMainM1Mirror.h"
#include "beamline/BioXAS/BioXASMainM1MirrorBendControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

BioXASMainM1Mirror::BioXASMainM1Mirror(QObject *parent) :
	BioXASM1Mirror("MainM1Mirror", parent)
{
	setUpstreamLength(-543.725);
	setDownstreamLength(543.725);

	setUpstreamInboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-01"), QString("SMTR1607-5-I21-01 VERT INB (UPSTREAM)"), true, -619.125, 190.438, 0.05, 2.0, this, QString(":mm")));
	setUpstreamOutboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-02"), QString("SMTR1607-5-I21-02 VERT OUTB (UPSTREAM)"), true, -619.125, -12.763, 0.05, 2.0, this, QString(":mm")));
	setDownstreamMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-03"), QString("SMTR1607-5-I21-03 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm")));
	setStripeSelectMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-04 STRIPE SELECT"), QString("SMTR1607-5-I21-04"), QString("SMTR1607-5-I21-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm")));
	setYawMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-05 YAW"), QString("SMTR1607-5-I21-05"), QString("SMTR1607-5-I21-05 YAW"), true, 0.05, 2.0, this, QString(":mm")));
	setUpstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-06"), QString("SMTR1607-5-I21-06 BENDER (UPSTREAM)"), true, 0.05, 2.0, this, QString(":lbs")));
	setDownstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-07"), QString("SMTR1607-5-I21-07 BENDER (DOWNSTREAM)"), true, 0.1, 2.0, this, QString(":lbs")));

	setPitch(new BioXASMirrorPitchControl(name()+"PitchControl", "deg", this));
	setRoll(new BioXASMirrorRollControl(name()+"RollControl", "deg", this));
	setHeight(new BioXASMirrorHeightControl(name()+"HeightControl", "mm", this));
	setLateral(new BioXASMirrorLateralControl(name()+"LateralControl", "mm", this));
	setYaw(new BioXASMirrorYawControl(name()+"YawControl", "deg", this));
	setBend(new BioXASMainM1MirrorBendControl(name()+"BendControl", "m", this));

	setUpperSlitBladeMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-08 UPPER SLIT"), QString("SMTR1607-5-I21-08"), QString("SMTR1607-5-I21-08 UPPER SLIT"), true, 0.1, 2.0, this, QString(":mm")));
}

BioXASMainM1Mirror::~BioXASMainM1Mirror()
{

}
