#include "AMSGMBeamline.h"

AMSGMBeamline* AMSGMBeamline::instance_ = 0;


AMSGMBeamline::AMSGMBeamline() : AMControl("AMSGMBeamline", "n/a") {

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::ringCurrent, this);
    addChild(ringCurrent_);
	energy_ = new AMPVwStatusControl("energy", "dave:Energy", "dave:Energy", "dave:Energy:moving", this, 0.01);
	AMReadOnlyPVwStatusControl *mono = new AMReadOnlyPVwStatusControl("mono", "dave:Energy:mono", "dave:Energy:mono:moving", energy_);
	AMReadOnlyPVwStatusControl *undulator = new AMReadOnlyPVwStatusControl("undulator", "dave:Energy:undulator", "dave:Energy:undulator:moving", energy_);
	AMReadOnlyPVwStatusControl *exitSlit = new AMReadOnlyPVwStatusControl("exitSlit", "dave:Energy:exitSlit", "dave:Energy:exitSlit:moving", energy_);
    energy_->addChild(mono);
    energy_->addChild(undulator);
    energy_->addChild(exitSlit);
    addChild(energy_);
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", "dave:Slit", "dave:Slit", "dave:Slit:moving", this, 0.1);
    addChild(exitSlitGap_);
	m4_ = new AMReadOnlyPVwStatusControl("M4", "dave:M4", "dave:M4:moving", this);
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", "dave:M4:inboard", "dave:M4:inboard", "dave:M4:inboard:moving", this, 0.1);
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", "dave:M4:outboard", "dave:M4:outboard", "dave:M4:outboard:moving", this, 0.1);
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", "dave:M4:downstream", "dave:M4:downstream", "dave:M4:downstream:moving", this, 0.1);
    m4_->addChild(m4inboard);
    m4_->addChild(m4outboard);
    m4_->addChild(m4downstream);
    addChild(m4_);
	grating_ = new AMPVwStatusControl("grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating:moving", this, 0.1);
    addChild(grating_);
	undulatorTracking_ = new AMPVControl("undulatorTracking", "dave:Energy:undulator:tracking", "dave:Energy:undulator:tracking", this, 0.1);
    addChild(undulatorTracking_);
	monoTracking_ = new AMPVControl("monoTracking", "dave:Energy:mono:tracking", "dave:Energy:mono:tracking", this, 0.1, 10);
    addChild(monoTracking_);
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", "dave:Energy:exitSlit:tracking", "dave:Energy:exitSlit:tracking", this, 0.1);
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
