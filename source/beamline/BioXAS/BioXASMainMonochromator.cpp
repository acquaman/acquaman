#include "BioXASMainMonochromator.h"

BioXASMainMonochromator::BioXASMainMonochromator(const QString &name, QObject *parent) :
        BioXASSSRLMonochromator(name, parent)
{
	// Mask controls.

	BioXASMAXvMotor *upperBlade = new BioXASMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	upperBlade->setLimitSetpoint(CLSMAXvMotor::LimitCW, BIOXASMAINMONOCHROMATOR_MASK_UPPER_BLADE_CW_DESTINATION);
	setUpperBlade(upperBlade);

	BioXASMAXvMotor *lowerBlade = new BioXASMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	lowerBlade->setLimitSetpoint(CLSMAXvMotor::LimitCW, BIOXASMAINMONOCHROMATOR_MASK_LOWER_BLADE_CW_DESTINATION);
	setLowerBlade(lowerBlade);

	setBladesState(new AMReadOnlyPVControl(QString("BL1607-5-I21:Mono:SlitsClosed"), QString("BL1607-5-I21:Mono:SlitsClosed"), this));

	// Paddle controls.

	BioXASMAXvMotor *paddle = new BioXASMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), false, 0.1, 2.0, this);
	paddle->setLimitSetpoint(CLSMAXvMotor::LimitCCW, BIOXASMAINMONOCHROMATOR_PADDLE_CCW_DESTINATION);
	setPaddle(paddle);

        setPaddleStatus(new AMReadOnlyPVControl(QString("%1PaddleStatus").arg(name), QString("BL1607-5-I21:Mono:PaddleExtracted"), this));

	// Key control.

        setKeyStatus(new AMReadOnlyPVControl(QString("%1KeyStatus").arg(name), QString("BL1607-5-I21:Mono:KeyStatus"), this));

	// Brake control.

        setBrakeStatus(new AMReadOnlyPVControl(QString("%1BrakeStatus").arg(name), QString("BL1607-5-I21:Mono:BrakeOff"), this));

	// Bragg controls.

        setBraggAtCrystalChangePositionStatus(new AMReadOnlyPVControl(QString("%1AtCrystalChangePosition").arg(name), "BL1607-5-I21:Mono:XtalChangePos", this));

	BioXASMAXvMotor *stepBragg = new BioXASMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), false, 0.05, 2.0, this, QString(":deg"));
	stepBragg->setLimitSetpoint(CLSMAXvMotor::LimitCW, BIOXASMAINMONOCHROMATOR_BRAGG_CW_DESTINATION);
	stepBragg->setLimitSetpoint(CLSMAXvMotor::LimitCCW, BIOXASMAINMONOCHROMATOR_BRAGG_CCW_DESTINATION);
	setStepBragg(stepBragg);

	BioXASMAXvMotor *encoderBragg = new BioXASMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	encoderBragg->setLimitSetpoint(CLSMAXvMotor::LimitCW, BIOXASMAINMONOCHROMATOR_BRAGG_CW_DESTINATION);
	encoderBragg->setLimitSetpoint(CLSMAXvMotor::LimitCCW, BIOXASMAINMONOCHROMATOR_BRAGG_CCW_DESTINATION);
	setEncoderBragg(encoderBragg);

	// Energy controls.

        setStepEnergy(new BioXASSSRLMonochromatorEnergyControl(QString("%1StepEnergyControl").arg(name), this));
        setEncoderEnergy(new BioXASSSRLMonochromatorEnergyControl(QString("%1EncoderEnergyControl").arg(name), this));

	// Crystal controls.

	setCrystal1Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal1Roll(new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Roll(new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V")));

	// Crystal change control.

	BioXASMAXvMotor *crystalChangeMotor = new BioXASMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystalChangeMotor->setLimitSetpoint(CLSMAXvMotor::LimitCW, BIOXASMAINMONOCHROMATOR_CRYSTAL_CHANGE_CW_DESTINATION);
	crystalChangeMotor->setLimitSetpoint(CLSMAXvMotor::LimitCCW, BIOXASMAINMONOCHROMATOR_CRYSTAL_CHANGE_CCW_DESTINATION);
	setCrystalChange(crystalChangeMotor);

	// Region controls.

        setRegionAStatus(new AMReadOnlyPVControl(QString("%1RegionAStatus").arg(name), "BL1607-5-I21:Mono:Region:A", this));
        setRegionBStatus(new AMReadOnlyPVControl(QString("%1RegionBStatus").arg(name), "BL1607-5-I21:Mono:Region:B", this));

        setRegion(new BioXASSSRLMonochromatorRegionControl(QString("%1Region").arg(name), this));

	// Vertical control.

	setVertical(new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this));

	// Lateral control.

	setLateral(new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this));

	// Misc.

        encoderStepsDiff_ = new AMReadOnlyPVControl(QString("%1EncoderStepsDiffControlDeg").arg(name), "BL1607-7-I21:Mono:fbk:diff", this);
}

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}
