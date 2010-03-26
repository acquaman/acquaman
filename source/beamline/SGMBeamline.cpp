#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : Control("SGMBeamline", "n/a") {

    ringCurrent_ = new ReadOnlyPVControl("ringCurrent", PVNames::ringCurrent, "", this);
    addChild(ringCurrent_);
    energy_ = new PVControl("energy", "dave:Energy", "dave:Energy", "dave:Energy:moving", 0.01, this);
    ReadOnlyPVControl *mono = new ReadOnlyPVControl("mono", "dave:Energy:mono", "dave:Energy:mono:moving", energy_);
    ReadOnlyPVControl *undulator = new ReadOnlyPVControl("undulator", "dave:Energy:undulator", "dave:Energy:undulator:moving", energy_);
    ReadOnlyPVControl *exitSlit = new ReadOnlyPVControl("exitSlit", "dave:Energy:exitSlit", "dave:Energy:exitSlit:moving", energy_);
    energy_->addChild(mono);
    energy_->addChild(undulator);
    energy_->addChild(exitSlit);
    addChild(energy_);
    exitSlitGap_ = new PVControl("exitSlitGap", "dave:Slit", "dave:Slit", "dave:Slit:moving", 0.1, this);
    addChild(exitSlitGap_);
    m4_ = new ReadOnlyPVControl("M4", "dave:M4", "dave:M4:moving", this);
    PVControl *m4inboard = new PVControl("M4Inboard", "dave:M4:inboard", "dave:M4:inboard", "dave:M4:inboard:moving", 0.1, this);
    PVControl *m4outboard = new PVControl("M4Outboard", "dave:M4:outboard", "dave:M4:outboard", "dave:M4:outboard:moving", 0.1, this);
    PVControl *m4downstream = new PVControl("M4Downstream", "dave:M4:downstream", "dave:M4:downstream", "dave:M4:downstream:moving", 0.1, this);
    m4_->addChild(m4inboard);
    m4_->addChild(m4outboard);
    m4_->addChild(m4downstream);
    addChild(m4_);
    grating_ = new PVControl("grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating:moving", -1, this);
    addChild(grating_);
}

SGMBeamline::~SGMBeamline()
{
}

SGMBeamline* SGMBeamline::sgm() {

        if(instance_ == 0)
                instance_ = new SGMBeamline();

        return instance_;

}

void SGMBeamline::releaseSGM() {

        if(instance_) {
                delete instance_;
                instance_ = 0;
        }

}
