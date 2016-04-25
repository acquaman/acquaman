#include "BioXASSideM1Mirror.h"
#include "beamline/BioXAS/BioXASSideM1MirrorBendControl.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"
#include "beamline/BioXAS/BioXASMirrorMotor.h"

BioXASSideM1Mirror::BioXASSideM1Mirror(QObject *parent) :
	BioXASM1Mirror("SideM1Mirror", parent)
{
	setUpstreamLength(-543.77);
	setDownstreamLength(543.68);

	CLSMAXvMotor *upperSlitBladeMotor = new CLSMAXvMotor(QString("SMTR1607-5-I22-08 UPPER SLIT"), QString("SMTR1607-5-I22-08"), QString("SMTR1607-5-I22-08 UPPER SLIT"), true, 0.05, 2.0, this, QString(":mm"));
	upperSlitBladeMotor->setLowLimitValue(0);
	upperSlitBladeMotor->setHighLimitValue(3.4);

	BioXASM1MirrorMask *mask = new BioXASM1MirrorMask(name()+"Mask", this);
	mask->setUpperSlitBlade(upperSlitBladeMotor);
	setMask(mask);

	setUpstreamInboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), QString("SMTR1607-5-I22-01"), QString("SMTR1607-5-I22-01 VERT INB (UPSTREAM)"), true, -619.125, 190.438, 0.05, 2.0, this, QString(":mm")));
	setUpstreamOutboardMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), QString("SMTR1607-5-I22-02"), QString("SMTR1607-5-I22-02 VERT OUTB (UPSTREAM)"), true, -619.125, -12.763, 0.05, 2.0, this, QString(":mm")));
	setDownstreamMotor(new BioXASMirrorMotor(QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), QString("SMTR1607-5-I22-03"), QString("SMTR1607-5-I22-03 VERT (DOWNSTREAM)"), true, 619.125, 0, 0.05, 2.0, this, QString(":mm")));
	setStripeSelectMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-04 STRIPE SELECT"), QString("SMTR1607-5-I22-04"), QString("SMTR1607-5-I22-04 STRIPE SELECT"), true, 0.05, 2.0, this, QString(":mm")));
	setYawMotor(new CLSMAXvMotor(QString("SMTR1607-5-I22-05 YAW"), QString("SMTR1607-5-I22-05"), QString("SMTR1607-5-I22-05 YAW"), true, 0.05, 2.0, this, QString(":mm")));

	CLSMAXvMotor *upstreamBender = new CLSMAXvMotor(QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), QString("SMTR1607-5-I22-06"), QString("SMTR1607-5-I22-06 BENDER (UPSTREAM)"), true, 0.3, 2.0, this, QString(":lbs"));
	upstreamBender->setLowLimitValue(0);
	upstreamBender->setHighLimitValue(10);
	setUpstreamBenderMotor(upstreamBender);

	CLSMAXvMotor *downstreamBender = new CLSMAXvMotor(QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), QString("SMTR1607-5-I22-07"), QString("SMTR1607-5-I22-07 BENDER (DOWNSTREAM)"), true, 0.3, 2.0, this, QString(":lbs"));
	downstreamBender->setLowLimitValue(0);
	downstreamBender->setHighLimitValue(10);
	setDownstreamBenderMotor(downstreamBender);

	BioXASMirrorPitchControl *pitchControl = new BioXASMirrorPitchControl(name()+"PitchControl", "deg", this);
	pitchControl->setMinimumValue(0.1);
	pitchControl->setMaximumValue(0.225);
	setPitch(pitchControl);

	BioXASMirrorRollControl *rollControl = new BioXASMirrorRollControl(name()+"RollControl", "deg", this);
	rollControl->setMinimumValue(-0.1);
	rollControl->setMaximumValue(0.1);
	setRoll(rollControl);

	BioXASMirrorHeightControl *heightControl = new BioXASMirrorHeightControl(name()+"HeightControl", "mm", this);
	heightControl->setMinimumValue(-2);
	heightControl->setMaximumValue(2);
	setHeight(heightControl);

	BioXASMirrorLateralControl *lateralControl = new BioXASMirrorLateralControl(name()+"LateralControl", "mm", this);
	lateralControl->setMinimumValue(-3);
	lateralControl->setMaximumValue(3);
	setLateral(lateralControl);

	BioXASMirrorYawControl *yawControl = new BioXASMirrorYawControl(name()+"YawControl", "deg", this);
	yawControl->setMinimumValue(-0.05);
	yawControl->setMaximumValue(0.05);
	setYaw(yawControl);

	setBend(new BioXASSideM1MirrorBendControl(name()+"BendControl", "m", this));

	setDownstreamBladeCurrent(new AMReadOnlyPVControl(QString("%1%2").arg(name()).arg("DownstreamBladeCurrent"), QString("A1607-5-03:A:fbk"), this));
}

BioXASSideM1Mirror::~BioXASSideM1Mirror()
{

}
