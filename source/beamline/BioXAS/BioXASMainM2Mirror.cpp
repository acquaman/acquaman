#include "BioXASMainM2Mirror.h"
#include "beamline/BioXAS/BioXASMainM2MirrorBendControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

BioXASMainM2Mirror::BioXASMainM2Mirror(QObject *parent) :
	BioXASM2Mirror("MainM2Mirror", parent)
{
	setUpstreamLength(-543.77);
	setDownstreamLength(543.68);

	setUpstreamInboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I21-15"), QString("SMTR1607-5-I21-15 VERT INB (UPSTREAM)"), true, -619.125, 61.933, 0.05, 2.0, this, QString(":mm")));
	setUpstreamOutboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I21-16"), QString("SMTR1607-5-I21-16 VERT OUTB (UPSTREAM)"), true, -619.125, -141.268, 0.05, 2.0, this, QString(":mm")));
	setDownstreamMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I21-17"), QString("SMTR1607-5-I21-17 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm")));
	setYawMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-19 YAW"), QString("SMTR1607-5-I21-19"), QString("SMTR1607-5-I21-19 YAW"), true, 0.05, 2.0, this, QString(":mm")));
	setLateralMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-18 STRIPE SELECT"), QString("SMTR1607-5-I21-18"), QString("SMTR1607-5-I21-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm")));
	setUpstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I21-20"), QString("SMTR1607-5-I21-20 BENDER (UPSTREAM)"), true, 0.1, 2.0, this, QString(":lbs")));
	setDownstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I21-21"), QString("SMTR1607-5-I21-21 BENDER (DOWNSTREAM)"), true, 0.1, 2.0, this, QString(":lbs")));

	setPitch(new BioXASMirrorPitchControl(name()+"PitchControl", "deg", this));
	setRoll(new BioXASMirrorRollControl(name()+"RollControl", "deg", this));
	setHeight(new BioXASMirrorHeightControl(name()+"HeightControl", "mm", this));
	setLateral(new BioXASMirrorLateralControl(name()+"LateralControl", "mm", this));
	setYaw(new BioXASMirrorYawControl(name()+"YawControl", "deg", this));
	setBend(new BioXASMainM2MirrorBendControl(name()+"BendControl", "m", this));

	setScreen(new AMSinglePVControl("M2FluorescentScreen", "VSC1607-5-I21-02:InBeam", this));
}

BioXASMainM2Mirror::~BioXASMainM2Mirror()
{

}
