#include "AMSGMBeamline.h"

AMSGMBeamline* AMSGMBeamline::instance_ = 0;


AMSGMBeamline::AMSGMBeamline() : AMControl("AMSGMBeamline", "n/a") {

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::ringCurrent, this);
    addChild(ringCurrent_);
	energy_ = new AMPVControl("energy", "dave:Energy", "dave:Energy", /*"dave:Energy:moving"*/ 0.01, 10.0, this);
	AMReadOnlyPVControl *mono = new AMReadOnlyPVControl("mono", "dave:Energy:mono", /*"dave:Energy:mono:moving",*/ energy_);
	AMReadOnlyPVControl *undulator = new AMReadOnlyPVControl("undulator", "dave:Energy:undulator", /*"dave:Energy:undulator:moving",*/ energy_);
	AMReadOnlyPVControl *exitSlit = new AMReadOnlyPVControl("exitSlit", "dave:Energy:exitSlit", /*"dave:Energy:exitSlit:moving",*/ energy_);
    energy_->addChild(mono);
    energy_->addChild(undulator);
    energy_->addChild(exitSlit);
    addChild(energy_);
	exitSlitGap_ = new AMPVControl("exitSlitGap", "dave:Slit", "dave:Slit", /*"dave:Slit:moving",*/ 0.1, 10.0, this);
    addChild(exitSlitGap_);
	m4_ = new AMReadOnlyPVControl("M4", "dave:M4", /*"dave:M4:moving",*/ this);
	AMPVControl *m4inboard = new AMPVControl("M4Inboard", "dave:M4:inboard", "dave:M4:inboard", /*"dave:M4:inboard:moving",*/ 0.1, 10, this);
	AMPVControl *m4outboard = new AMPVControl("M4Outboard", "dave:M4:outboard", "dave:M4:outboard", /*"dave:M4:outboard:moving",*/ 0.1, 10, this);
	AMPVControl *m4downstream = new AMPVControl("M4Downstream", "dave:M4:downstream", "dave:M4:downstream", /*"dave:M4:downstream:moving",*/ 0.1, 10, this);
    m4_->addChild(m4inboard);
    m4_->addChild(m4outboard);
    m4_->addChild(m4downstream);
    addChild(m4_);
    grating_ = new AMPVControl("grating", "dave:Energy:mono:grating", "dave:Energy:mono:grating", /*"dave:Energy:mono:grating:moving",*/ 0.1, 10, this);
    grating_->setDiscrete(true);
    addChild(grating_);
    harmonic_ = new AMPVControl("harmonic", "dave:Energy:undulator:harmonic", "dave:Energy:undulator:harmonic", 0.1, 10, this);
    harmonic_->setDiscrete(true);
    addChild(harmonic_);
    undulatorTracking_ = new AMPVControl("undulatorTracking", "dave:Energy:undulator:tracking", "dave:Energy:undulator:tracking", 0.1, 10, this);
    undulatorTracking_->setDiscrete(true);
    addChild(undulatorTracking_);
    monoTracking_ = new AMPVControl("monoTracking", "dave:Energy:mono:tracking", "dave:Energy:mono:tracking", 0.1, 10, this);
    monoTracking_->setDiscrete(true);
    addChild(monoTracking_);
    exitSlitTracking_ = new AMPVControl("exitSlitTracking", "dave:Energy:exitSlit:tracking", "dave:Energy:exitSlit:tracking", 0.1, 10, this);
    exitSlitTracking_->setDiscrete(true);
    addChild(exitSlitTracking_);
}

AMSGMBeamline::~AMSGMBeamline()
{
}

bool AMSGMBeamline::energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy){
    if( (grating == 0) && (harmonic == 1) && (energy > 240) && (energy < 750) )
        return true;
    else if( (grating == 1) && (harmonic == 1) && (energy > 440) && (energy < 1200) )
        return true;
    else if( (grating == 2) && (harmonic == 1) && (energy > 800) && (energy < 1100) )
        return true;
    else if( (grating == 2) && (harmonic == 3) && (energy > 1100) && (energy < 2000) )
        return true;
    else
        return false;
}

bool AMSGMBeamline::energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy){
    if( (grating == 0) && (harmonic == 1) && (maxEnergy > 240) && (minEnergy < 750) )
        return true;
    else if( (grating == 1) && (harmonic == 1) && (maxEnergy > 440) && (minEnergy < 1200) )
        return true;
    else if( (grating == 2) && (harmonic == 1) && (maxEnergy > 800) && (minEnergy < 1100) )
        return true;
    else if( (grating == 2) && (harmonic == 3) && (maxEnergy > 1100) && (minEnergy < 2000) )
        return true;
    else
        return false;
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



AMSGMFluxOptimization::AMSGMFluxOptimization(QObject *parent) : AMControlOptimization(parent) {
    name_ = "SGMFlux";
}

QMap<double, double> AMSGMFluxOptimization::curve(QList<QVariant> stateParameters, QList<AMXASRegion*> contextParameters){
    double _maxenergy = maximumEnergy(contextParameters);
    double _minenergy = minimumEnergy(contextParameters);
    double _maxflux = 0;
    double _minflux = 0;
    double _maximum = 0;
    double _minimum = 1;
    double _slit = stateParameters.at(0).toDouble();
    AMSGMBeamline::sgmGrating _grating = (AMSGMBeamline::sgmGrating)stateParameters.at(1).toInt();
    AMSGMBeamline::sgmHarmonic _harmonic = (AMSGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
    if(!AMSGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
    {
    }
    else if((_harmonic == 3) && (_grating == 2)){

        _maxflux = 1.2;
        _minflux = 1.05;
        _maximum = 1600;
        _minimum = 1100;
    }
    else{
        if( (_grating == 0) && AMSGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
            _maxflux = 5.75;
            _minflux = 0.5;
            _maximum = 475;
            _minimum = 200;
        }
        else if( (_grating == 1) && AMSGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
            _maxflux = 3.25;
            _minflux = 0.5;
            _maximum = 815;
            _minimum = 450;
        }
        else if( (_grating == 2) && AMSGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
            _maxflux = 1.1;
            _minflux = 0.5;
            _maximum = 1075;
            _minimum = 800;
        }
    }
    QMap<double, double> rCurve;
    double tmpStart, tmpEnd, tmpDelta;
    for( int x = 0; x < contextParameters.count(); x++){
        tmpStart = contextParameters.at(x)->start();
        tmpDelta = contextParameters.at(x)->delta();
        tmpEnd = contextParameters.at(x)->end();
        for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
            rCurve[y] = !AMSGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
        }
    }
    return rCurve;
}

double AMSGMFluxOptimization::maximumEnergy(QList<AMXASRegion*> regions){
    double curMax = 240;
    if(regions.count() == 0)
        return -1;
    for(int x = 0; x < regions.count(); x++){
        curMax = (regions.at(x)->start() > curMax ? regions.at(x)->start() : curMax);
        curMax = (regions.at(x)->end() > curMax ? regions.at(x)->end() : curMax);
    }
    return curMax;
}

double AMSGMFluxOptimization::minimumEnergy(QList<AMXASRegion*> regions){
    double curMin = 2000;
    if(regions.count() == 0)
        return -1;
    for(int x = 0; x < regions.count(); x++){
        curMin = (regions.at(x)->start() < curMin ? regions.at(x)->start() : curMin);
        curMin = (regions.at(x)->end() < curMin ? regions.at(x)->end() : curMin);
    }
    return curMin;
}
