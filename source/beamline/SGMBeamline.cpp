#include "SGMBeamline.h"

AMSGMBeamline* AMSGMBeamline::instance_ = 0;


AMSGMBeamline::AMSGMBeamline() : AMControl("AMSGMBeamline", "n/a") {

    ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::ringCurrent, "", this);
    addChild(ringCurrent_);
    energy_ = new AMPVControl("energy", "dave:Energy", "dave:Energy", "dave:Energy:moving", 0.01, this);
    AMReadOnlyPVControl *mono = new AMReadOnlyPVControl("mono", "dave:Energy:mono", "dave:Energy:mono:moving", energy_);
    AMReadOnlyPVControl *undulator = new AMReadOnlyPVControl("undulator", "dave:Energy:undulator", "dave:Energy:undulator:moving", energy_);
    AMReadOnlyPVControl *exitSlit = new AMReadOnlyPVControl("exitSlit", "dave:Energy:exitSlit", "dave:Energy:exitSlit:moving", energy_);
    energy_->addChild(mono);
    energy_->addChild(undulator);
    energy_->addChild(exitSlit);
    addChild(energy_);
    exitSlitGap_ = new AMPVControl("exitSlitGap", "dave:Slit", "dave:Slit", "dave:Slit:moving", 0.1, this);
    addChild(exitSlitGap_);
    m4_ = new AMReadOnlyPVControl("M4", "dave:M4", "dave:M4:moving", this);
    AMPVControl *m4inboard = new AMPVControl("M4Inboard", "dave:M4:inboard", "dave:M4:inboard", "dave:M4:inboard:moving", 0.1, this);
    AMPVControl *m4outboard = new AMPVControl("M4Outboard", "dave:M4:outboard", "dave:M4:outboard", "dave:M4:outboard:moving", 0.1, this);
    AMPVControl *m4downstream = new AMPVControl("M4Downstream", "dave:M4:downstream", "dave:M4:downstream", "dave:M4:downstream:moving", 0.1, this);
    m4_->addChild(m4inboard);
    m4_->addChild(m4outboard);
    m4_->addChild(m4downstream);
    addChild(m4_);
    grating_ = new AMPVControl("grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating:moving", -1, this);
    addChild(grating_);
    undulatorTracking_ = new AMPVControl("undulatorTracking", "dave:Energy:undulator:tracking", "dave:Energy:undulator:tracking", "", -1, this);
    addChild(undulatorTracking_);
    monoTracking_ = new AMPVControl("monoTracking", "dave:Energy:mono:tracking", "dave:Energy:mono:tracking", "", -1, this);
    addChild(monoTracking_);
    exitSlitTracking_ = new AMPVControl("exitSlitTracking", "dave:Energy:exitSlit:tracking", "dave:Energy:exitSlit:tracking", "", -1, this);
    addChild(exitSlitTracking_);
}

AMSGMBeamline::~AMSGMBeamline()
{
}

AMSGMBeamline* AMSGMBeamline::sgm() {

        if(instance_ == 0)
                instance_ = new AMSGMBeamline();

        return instance_;

}

void AMSGMBeamline::releaseSGM() {

        if(instance_) {
                delete instance_;
                instance_ = 0;
        }

}
