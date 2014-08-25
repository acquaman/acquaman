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
    thetaPaddlePermitted_ = new AMReadOnlyPVControl("ThetaPaddlePermitted", "07ID_SideMono:ThetaPaddle:Permited", this);
    slitsCloseCmd_ = new AMSinglePVControl("SlitsCloseCmd", "07ID_SideMono:SlitsCloseCmd", this);
    slitsEnabled_ = new AMReadOnlyPVControl("SlitsEnabled", "07ID_SideMono:SlitsEnabled", this);

    connect( tankClosed_, SIGNAL(valueChanged(double)), this, SLOT(onTankClosedChanged(double)) );
    connect( brakeOff_, SIGNAL(valueChanged(double)), this, SLOT(onBrakeOffChanged(double)) );
    connect( keyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged(double)) );
    connect( paddleExtracted_, SIGNAL(valueChanged(double)), this, SIGNAL(onPaddleExtractedChanged(double)) );
    connect( crystalChangePosition_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangePositionChanged(double)) );
    connect( upperSlitClosed_, SIGNAL(valueChanged(double)), this, SLOT(onUpperSlitClosedChanged(double)) );
    connect( lowerSlitClosed_, SIGNAL(valueChanged(double)), this, SLOT(onLowerSlitClosedChanged(double)) );
    connect( slitsClosed_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsClosedChanged(double)) );
    connect( thetaACWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onThetaACWLimitChanged(double)) );
    connect( thetaACCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onThetaACCWLimitChanged(double)) );
    connect( thetaBCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onThetaBCWLimitChanged(double)) );
    connect( thetaBCCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onThetaBCCWLimitChanged(double)) );
    connect( regionA_, SIGNAL(valueChanged(double)), this, SLOT(onRegionAChanged(double)) );
    connect( regionB_, SIGNAL(valueChanged(double)), this, SLOT(onRegionBChanged(double)) );
    connect( translateALimit_, SIGNAL(valueChanged(double)), this, SLOT(onTranslateALimitChanged(double)) );
    connect( translateBLimit_, SIGNAL(valueChanged(double)), this, SLOT(onTranslateBLimitChanged(double)) );
    connect( translatePermitted_, SIGNAL(valueChanged(double)), this, SLOT(onTranslatePermittedChanged(double)) );
    connect( thetaPaddlePermitted_, SIGNAL(valueChanged(double)), this, SLOT(onThetaPaddlePermittedChanged(double)) );
    connect( slitsEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsEnabledChanged(double)) );
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

bool BioXASSideMonochromator::tankClosed() const
{
    return (int(tankClosed_->value()) == 0);
}

bool BioXASSideMonochromator::brakeOff() const
{
    return (int(brakeOff_->value()) == 0);
}

bool BioXASSideMonochromator::keyStatus() const
{
    return (int(keyStatus_->value()) == 0);
}

bool BioXASSideMonochromator::paddleExtracted() const
{
    return (int(paddleExtracted_->value()) == 0);
}

bool BioXASSideMonochromator::crystalChangePosition() const
{
    return (int(crystalChangePosition_->value()) == 0);
}

bool BioXASSideMonochromator::upperSlitClosed() const
{
    return (int(upperSlitClosed_->value()) == 0);
}

bool BioXASSideMonochromator::lowerSlitClosed() const
{
    return (int(lowerSlitClosed_->value()) == 0);
}

bool BioXASSideMonochromator::slitsClosed() const
{
    return (int(slitsClosed_->value()) == 0);
}

bool BioXASSideMonochromator::thetaACWLimit() const
{
    return (int(thetaACWLimit_->value()) == 0);
}

bool BioXASSideMonochromator::thetaACCWLimit() const
{
    return (int(thetaACCWLimit_->value()) == 0);
}

bool BioXASSideMonochromator::thetaBCWLimit() const
{
    return (int(thetaBCWLimit_->value()) == 0);
}

bool BioXASSideMonochromator::thetaBCCWLimit() const
{
    return (int(thetaBCCWLimit_->value()) == 0);
}

bool BioXASSideMonochromator::regionA() const
{
    return (int(regionA_->value()) == 0);
}

bool BioXASSideMonochromator::regionB() const
{
    return (int(regionB_->value()) == 0);
}

bool BioXASSideMonochromator::translateALimit() const
{
    return (int(translateALimit_->value()) == 0);
}

bool BioXASSideMonochromator::translateBLimit() const
{
    return (int(translateBLimit_->value()) == 0);
}

bool BioXASSideMonochromator::translatePermitted() const
{
    return (int(translatePermitted_->value()) == 0);
}

bool BioXASSideMonochromator::thetaPaddlePermitted() const
{
    return (int(thetaPaddlePermitted_->value()) == 0);
}

bool BioXASSideMonochromator::slitsEnabled() const
{
    return (int(slitsEnabled_->value()) == 0);
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

void BioXASSideMonochromator::onTankClosedChanged(double newValue)
{
    emit tankClosedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onBrakeOffChanged(double newValue)
{
    emit brakeOffStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onKeyStatusChanged(double newValue)
{
    emit keyStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onPaddleExtractedChanged(double newValue)
{
    emit paddleExtractedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onCrystalChangePositionChanged(double newValue)
{
    emit crystalChangePositionStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onUpperSlitClosedChanged(double newValue)
{
    emit upperSlitClosedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onLowerSlitClosedChanged(double newValue)
{
    emit lowerSlitClosedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onSlitsClosedChanged(double newValue)
{
    emit slitsClosedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onThetaACWLimitChanged(double newValue)
{
    emit thetaACWLimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onThetaACCWLimitChanged(double newValue)
{
    emit thetaACCWLimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onThetaBCWLimitChanged(double newValue)
{
    emit thetaBCWLimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onThetaBCCWLimitChanged(double newValue)
{
    emit thetaBCCWLimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onRegionAChanged(double newValue)
{
    emit regionAStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onRegionBChanged(double newValue)
{
    emit regionBStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onTranslateALimitChanged(double newValue)
{
    emit translateALimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onTranslateBLimitChanged(double newValue)
{
    emit translateBLimitStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onTranslatePermittedChanged(double newValue)
{
    emit translatePermittedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onThetaPaddlePermittedChanged(double newValue)
{
    emit thetaPaddlePermittedStatusChanged( (int(newValue) == 0) );
}

void BioXASSideMonochromator::onSlitsEnabledChanged(double newValue)
{
    emit slitsEnabledStatusChanged( (int(newValue) == 0) );
}


