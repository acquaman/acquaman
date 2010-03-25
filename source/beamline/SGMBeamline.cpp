#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : Control("SGMBeamline", "n/a") {

    ringCurrent_ = new ReadOnlyPVControl("ringCurrent", PVNames::ringCurrent, "", this);
//    children_ << ringCurrent_;
    addChild(ringCurrent_);
    energy_ = new PVControl("energy", "dave:Energy", "dave:Energy", "dave:Energy:moving", 0.01, this);
//    children_ << energy_;
    addChild(energy_);
    exitSlitGap_ = new PVControl("exitSlitGap", "dave:Slit", "dave:Slit", "dave:Slit:moving", 0.1, this);
//    children_ << exitSlitGap_;
    addChild(exitSlitGap_);
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
