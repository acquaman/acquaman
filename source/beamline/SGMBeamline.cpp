#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : Control("SGMBeamline", "n/a") {

    ringCurrent_ = new ReadOnlyPVControl("ringCurrent", PVNames::ringCurrent, "", this);
    children_ << ringCurrent_;
    energy_ = new PVControl("energy", "BL1611-ID-1:Energy", "BL1611-ID-1:Energy", "BL1611-ID-1:ready", 0.01, this);
    children_ << energy_;
    exitSlitGap_ = new PVControl("exitSlitGap", "PSL16114I1004:Y:mm:encsp", "PSL16114I1004:Y:mm:encsp", "SMTR16114I1017:status", 0.1, this);
    children_ << exitSlitGap_;
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
