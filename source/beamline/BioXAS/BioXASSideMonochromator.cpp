#include "BioXASSideMonochromator.h"

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
    QObject(parent)
{
    tankClosed_ = new AMProcessVariable("07ID_MainMono:TankClosed", true, this);
    brakeOff_ = new AMProcessVariable("07ID_MainMono:BrakeOff", true, this);
    keyStatus_ = new AMProcessVariable("07ID_MainMono:KeyStatus", true, this);
    paddleExtracted_ = new AMProcessVariable("07ID_MainMono:PaddleExtracted", true, this);
    crystalChangePosition_ = new AMProcessVariable("07ID_MainMono:XtalChangePos", true, this);
    upperSlitClosed_ = new AMProcessVariable("07ID_MainMono:UpperSlitClosed", true, this);
    lowerSlitClosed_ = new AMProcessVariable("07ID_MainMono:LowerSlitClosed", true, this);
    slitsClosed_ = new AMProcessVariable("07ID_MainMono:SlitsClosed", true, this);
    thetaACWLimit_ = new AMProcessVariable("07ID_MainMono:Theta:A:CWLIM", true, this);
    thetaACCWLimit_ = new AMProcessVariable("07ID_MainMono:Theta:A:CCWLIM", true, this);
    thetaBCWLimit_ = new AMProcessVariable("07ID_MainMono:Theta:B:CWLIM", true, this);
    thetaBCCWLimit_ = new AMProcessVariable("07ID_MainMono:Theta:B:CCWLIM", true, this);
    regionA_ = new AMProcessVariable("07ID_MainMono:Region:A", true, this);
    regionB_ = new AMProcessVariable("07ID_MainMono:Region:B", true, this);
    translateALimit_ = new AMProcessVariable("07ID_MainMono:Translate:A:Limit", true, this);
    translateBLimit_ = new AMProcessVariable("07ID_MainMono:Translate:B:Limit", true, this);
    translatePermitted_ = new AMProcessVariable("07ID_MainMono:Translate:Permited", true, this);
    slitsCloseCmd_ = new AMProcessVariable("07ID_MainMono:SlitsCloseCmd", true, this);
    slitsEnabled_ = new AMProcessVariable("07ID_MainMono:SlitsEnabled", true, this);
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}


