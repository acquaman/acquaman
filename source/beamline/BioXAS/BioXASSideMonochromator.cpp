#include "BioXASSideMonochromator.h"
#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
    QObject(parent)
{
    tankClosed_ = new AMReadOnlyPVControl("TankClosed", "07ID_SideMono:TankClosed", this);
    brakeOff_ = new AMReadOnlyPVControl("BrakeOff", "07ID_SideMono:BrakeOff", this);
    keyStatus_ = new AMReadOnlyPVControl("KeyStatus", "07ID_SideMono:KeyStatus", this);
    paddleExtracted_ = new AMReadOnlyPVControl("PaddleExtracted", "07ID_SideMono:PaddleExtracted", this);
    crystalChangePosition_ = new AMReadOnlyPVControl("CrystalChangePosition", "07ID_SideMono:XtalChangePos", this);
    upperSlitClosed_ = new AMReadOnlyPVControl("UpperSlitClosed", "07ID_SideMono:UpperSlitClosed", this);
    lowerSlitClosed_ = new AMReadOnlyPVControl("LowerSlitClosed", "07ID_SideMono:LowerSlitClosed", this);
    slitsClosed_ = new AMReadOnlyPVControl("SlitsClosed", "07ID_SideMono:SlitsClosed", this);
    thetaACWLimit_ = new AMReadOnlyPVControl("ThetaACWLimit", "07ID_SideMono:Theta:A:CWLIM", this);
    thetaACCWLimit_ = new AMReadOnlyPVControl("ThetaACCWLimit", "07ID_SideMono:Theta:A:CCWLIM", this);
    thetaBCWLimit_ = new AMReadOnlyPVControl("ThetaBCWLimit", "07ID_SideMono:Theta:B:CWLIM", this);
    thetaBCCWLimit_ = new AMReadOnlyPVControl("ThetaBCCWLimit", "07ID_SideMono:Theta:B:CCWLIM", this);
    regionA_ = new AMReadOnlyPVControl("RegionA", "07ID_SideMono:Region:A", this);
    regionB_ = new AMReadOnlyPVControl("RegionB", "07ID_SideMono:Region:B", this);
    translateALimit_ = new AMReadOnlyPVControl("TranslateALimit", "07ID_SideMono:Translate:A:Limit", this);
    translateBLimit_ = new AMReadOnlyPVControl("TranslateBLimit", "07ID_SideMono:Translate:B:Limit", this);
    translatePermitted_ = new AMReadOnlyPVControl("TranslatePermitted", "07ID_SideMono:Translate:Permited", this);
    slitsCloseCmd_ = new AMSinglePVControl("SlitsCloseCmd", "07ID_SideMono:SlitsCloseCmd", this);
    slitsEnabled_ = new AMReadOnlyPVControl("SlitsEnabled", "07ID_SideMono:SlitsEnabled", this);

    connect( tankClosed_, SIGNAL(valueChanged(double)), this, SIGNAL(tankClosedChanged(double)) );
    connect( brakeOff_, SIGNAL(valueChanged(double)), this, SIGNAL(brakeOffChanged(double)) );
    connect( keyStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(keyStatusChanged(double)) );
    connect( paddleExtracted_, SIGNAL(valueChanged(double)), this, SIGNAL(paddleExtractedChanged(double)) );
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

bool BioXASSideMonochromator::tankClosed() const
{
    return (tankClosed_->value() == 0);
}

bool BioXASSideMonochromator::brakeOff() const
{
    return (brakeOff_->value() == 0);
}

bool BioXASSideMonochromator::keyStatus() const
{
    return (keyStatus_->value() == 0);
}

bool BioXASSideMonochromator::paddleExtracted() const
{
    return (paddleExtracted_->value() == 0);
}

bool BioXASSideMonochromator::crystalChangePosition() const
{
    return (crystalChangePosition_->value() == 0);
}

bool BioXASSideMonochromator::upperSlitClosed() const
{
    return (upperSlitClosed_->value() == 0);
}

bool BioXASSideMonochromator::lowerSlitClosed() const
{
    return (lowerSlitClosed_->value() == 0);
}

bool BioXASSideMonochromator::slitsClosed() const
{
    return (slitsClosed_->value() == 0);
}

bool BioXASSideMonochromator::thetaACWLimit() const
{
    return (thetaACWLimit_->value() == 0);
}

bool BioXASSideMonochromator::thetaACCWLimit() const
{
    return (thetaACCWLimit_->value() == 0);
}

bool BioXASSideMonochromator::thetaBCWLimit() const
{
    return (thetaBCWLimit_->value() == 0);
}

bool BioXASSideMonochromator::thetaBCCWLimit() const
{
    return (thetaBCCWLimit_->value() == 0);
}

bool BioXASSideMonochromator::regionA() const
{
    return (regionA_->value() == 0);
}

bool BioXASSideMonochromator::regionB() const
{
    return (regionB_->value() == 0);
}

bool BioXASSideMonochromator::translateALimit() const
{
    return (translateALimit_->value() == 0);
}

bool BioXASSideMonochromator::translateBLimit() const
{
    return (translateBLimit_->value() == 0);
}

bool BioXASSideMonochromator::translatePermitted() const
{
    return (translatePermitted_->value() == 0);
}

bool BioXASSideMonochromator::thetaPaddlePermitted() const
{
    return (thetaPaddlePermitted_->value() == 0);
}

bool BioXASSideMonochromator::slitsEnabled() const
{
    return (slitsEnabled_->value() == 0);
}

AMAction3* BioXASSideMonochromator::createCloseSlitsAction()
{
    if (!slitsCloseCmd_->isConnected())
        return 0;

    AMControlInfo setpoint = slitsCloseCmd_->toInfo();
    setpoint.setValue(0);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMAction3 *action = new AMControlMoveAction3(actionInfo, slitsCloseCmd_);

    return action;
}


