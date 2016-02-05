#include "BioXASSideM2Mirror.h"
#include "beamline/BioXAS/BioXASSideM2MirrorBendControl.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

BioXASSideM2Mirror::BioXASSideM2Mirror(QObject *parent) :
	BioXASM2Mirror("SideM2Mirror", parent)
{
	setUpstreamLength(-543.725);
	setDownstreamLength(543.725);

	setUpstreamInboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-15"), QString("SMTR1607-5-I22-15 VERT INB (UPSTREAM)"), true, -619.125, 61.934, 0.05, 2.0, this, QString(":mm")));
	setUpstreamOutboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-16"), QString("SMTR1607-5-I22-16 VERT OUTB (UPSTREAM)"), true, -619.125, -141.266, 0.05, 2.0, this, QString(":mm")));
	setDownstreamMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-17"), QString("SMTR1607-5-I22-17 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm")));
	setStripeSelectMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-18 STRIPE SELECT"), QString("SMTR1607-5-I22-18"), QString("SMTR1607-5-I22-18 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm")));
	setYawMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-19 YAW"), QString("SMTR1607-5-I22-19"), QString("SMTR1607-5-I22-19 YAW"), true, 0.05, 2.0, this, QString(":mm")));
	setUpstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), true, 0.3, 2.0, this, QString(":lbs")));
	setDownstreamBenderMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), true, 0.3, 2.0, this, QString(":lbs")));

	setPitch(new BioXASMirrorPitchControl(name()+"PitchControl", "deg", this));
	setRoll(new BioXASMirrorRollControl(name()+"RollControl", "deg", this));
	setHeight(new BioXASMirrorHeightControl(name()+"HeightControl", "mm", this));
	setLateral(new BioXASMirrorLateralControl(name()+"LateralControl", "mm", this));
	setYaw(new BioXASMirrorYawControl(name()+"YawControl", "deg", this));
	setBend(new BioXASSideM2MirrorBendControl(name()+"BendControl", "m", this));

	setScreen(new AMSinglePVControl(name()+"FluorescentScreen", "VSC1607-5-I22-02:InBeam", this));
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
