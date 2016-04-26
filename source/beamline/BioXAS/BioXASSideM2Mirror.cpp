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

	CLSMAXvMotor *upstreamBender = new CLSMAXvMotor(QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-20"), QString("SMTR1607-5-I22-20 BENDER (UPSTREAM)"), true, 0.3, 2.0, this, QString(":lbs"));
	upstreamBender->setMinimumValue(0);
	upstreamBender->setMaximumValue(17);
	setUpstreamBenderMotor(upstreamBender);

	CLSMAXvMotor *downstreamBender = new CLSMAXvMotor(QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-21"), QString("SMTR1607-5-I22-21 BENDER (DOWNSTREAM)"), true, 0.3, 2.0, this, QString(":lbs"));
	downstreamBender->setMinimumValue(0);
	downstreamBender->setMaximumValue(17);
	setDownstreamBenderMotor(downstreamBender);

	BioXASMirrorPitchControl *pitchControl = new BioXASMirrorPitchControl(name()+"PitchControl", "deg", this);
	pitchControl->setMinimumValue(0.125);
	pitchControl->setMaximumValue(0.27);
	setPitch(pitchControl);

	BioXASMirrorRollControl *rollControl = new BioXASMirrorRollControl(name()+"RollControl", "deg", this);
	rollControl->setMinimumValue(-0.125);
	rollControl->setMaximumValue(0.125);
	setRoll(rollControl);

	BioXASMirrorHeightControl *heightControl = new BioXASMirrorHeightControl(name()+"HeightControl", "mm", this);
	heightControl->setMinimumValue(-2.5);
	heightControl->setMaximumValue(12.5);
	setHeight(heightControl);

	BioXASMirrorLateralControl *lateralControl = new BioXASMirrorLateralControl(name()+"LateralControl", "mm", this);
	lateralControl->setMinimumValue(-3);
	lateralControl->setMaximumValue(3);
	setLateral(lateralControl);

	BioXASMirrorYawControl *yawControl = new BioXASMirrorYawControl(name()+"YawControl", "deg", this);
	yawControl->setMinimumValue(-0.05);
	yawControl->setMaximumValue(0.05);
	setYaw(yawControl);

	setBend(new BioXASSideM2MirrorBendControl(name()+"BendControl", "m", this));

	setScreen(new AMSinglePVControl(name()+"FluorescentScreen", "VSC1607-5-I22-02:InBeam", this));
}

BioXASSideM2Mirror::~BioXASSideM2Mirror()
{

}
