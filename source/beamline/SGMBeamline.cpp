#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : AMControl("SGMBeamline", "n/a") {

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
	//grating_->setDiscrete(true);
    addChild(grating_);
    harmonic_ = new AMPVwStatusControl("harmonic", "dave:Energy:undulator:harmonic", "dave:Energy:undulator:harmonic", "dave:Energy:undulator:harmonic:moving", this, 0.1);
	//harmonic_->setDiscrete(true);
    addChild(harmonic_);
    undulatorTracking_ = new AMPVControl("undulatorTracking", "dave:Energy:undulator:tracking", "dave:Energy:undulator:tracking", this, 0.1);
	//undulatorTracking_->setDiscrete(true);
    addChild(undulatorTracking_);
    monoTracking_ = new AMPVControl("monoTracking", "dave:Energy:mono:tracking", "dave:Energy:mono:tracking", this, 0.1, 10);
	//monoTracking_->setDiscrete(true);
    addChild(monoTracking_);
    exitSlitTracking_ = new AMPVControl("exitSlitTracking", "dave:Energy:exitSlit:tracking", "dave:Energy:exitSlit:tracking", this, 0.1);
	//exitSlitTracking_->setDiscrete(true);
    addChild(exitSlitTracking_);

	fluxOptimization_ = new SGMFluxOptimization(this);
	resolutionOptimization_ = new SGMResolutionOptimization(this);
	fluxResolutionSet_ = new AMControlOptimizationSet(this);
	fluxResolutionSet_->setName("Flux and Resolution");
	fluxResolutionSet_->addControl(grating_);
	fluxResolutionSet_->addControl(harmonic_);
	fluxResolutionSet_->addControl(exitSlitGap_);
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(fluxOptimization_);

	trackingSet_ = new AMControlSet(this);
	trackingSet_->setName("Tracking");
	trackingSet_->addControl(undulatorTracking_);
	trackingSet_->addControl(monoTracking_);
	trackingSet_->addControl(exitSlitTracking_);

}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy){
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

bool SGMBeamline::energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy){
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



SGMFluxOptimization::SGMFluxOptimization(QObject *parent) : AMControlOptimization(parent) {
    name_ = "SGMFlux";
}

QMap<double, double> SGMFluxOptimization::curve(QList<QVariant> stateParameters, QList<AMXASRegion*> contextParameters){
    double _maxenergy = maximumEnergy(contextParameters);
    double _minenergy = minimumEnergy(contextParameters);
    double _maxflux = 0;
    double _minflux = 0;
    double _maximum = 0;
    double _minimum = 1;
    double _slit = stateParameters.at(0).toDouble();
    SGMBeamline::sgmGrating _grating = (SGMBeamline::sgmGrating)stateParameters.at(1).toInt();
    SGMBeamline::sgmHarmonic _harmonic = (SGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
    if(!SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
    {
    }
    else if((_harmonic == 3) && (_grating == 2)){

        _maxflux = 1.2;
        _minflux = 1.05;
        _maximum = 1600;
        _minimum = 1100;
    }
    else{
        if( (_grating == 0) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
            _maxflux = 5.75;
            _minflux = 0.5;
            _maximum = 475;
            _minimum = 200;
        }
        else if( (_grating == 1) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
            _maxflux = 3.25;
            _minflux = 0.5;
            _maximum = 815;
            _minimum = 450;
        }
        else if( (_grating == 2) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
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
            rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
        }
    }
    return rCurve;
}

double SGMFluxOptimization::maximumEnergy(QList<AMXASRegion*> regions){
    double curMax = 240;
    if(regions.count() == 0)
        return -1;
    for(int x = 0; x < regions.count(); x++){
        curMax = (regions.at(x)->start() > curMax ? regions.at(x)->start() : curMax);
        curMax = (regions.at(x)->end() > curMax ? regions.at(x)->end() : curMax);
    }
    return curMax;
}

double SGMFluxOptimization::minimumEnergy(QList<AMXASRegion*> regions){
    double curMin = 2000;
    if(regions.count() == 0)
        return -1;
    for(int x = 0; x < regions.count(); x++){
        curMin = (regions.at(x)->start() < curMin ? regions.at(x)->start() : curMin);
        curMin = (regions.at(x)->end() < curMin ? regions.at(x)->end() : curMin);
    }
    return curMin;
}


SGMResolutionOptimization::SGMResolutionOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMResolution";
}

QMap<double, double> SGMResolutionOptimization::curve(QList<QVariant> stateParameters, QList<AMXASRegion*> contextParameters){
	double _maxenergy = maximumEnergy(contextParameters);
	double _minenergy = minimumEnergy(contextParameters);
	double _maxflux = 0;
	double _minflux = 0;
	double _maximum = 0;
	double _minimum = 1;
	double _stateScalar = 0;
	double _slit = stateParameters.at(0).toDouble();
	SGMBeamline::sgmGrating _grating = (SGMBeamline::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamline::sgmHarmonic _harmonic = (SGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == 3) && (_grating == 2)){

		_maxflux = 1.75;
		_minflux = 0.5;
		_maximum = 1200;
		_minimum = 1100;
		_stateScalar = -6.0;
	}
	else{
		if( (_grating == 0) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 4.2;
			_minflux = 1.75;
			_maximum = 400;
			_minimum = 200;
			_stateScalar = 2.0;
		}
		else if( (_grating == 1) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 3.9;
			_minflux = 2.0;
			_maximum = 800;
			_minimum = 450;
			_stateScalar = -2.0;
		}
		else if( (_grating == 2) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 2.75;
			_minflux = 1.5;
			_maximum = 875;
			_minimum = 800;
			_stateScalar = -3.0;
		}
	}
	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta;
	for( int x = 0; x < contextParameters.count(); x++){
		tmpStart = contextParameters.at(x)->start();
		tmpDelta = contextParameters.at(x)->delta();
		tmpEnd = contextParameters.at(x)->end();
		for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
// FIX NEGATIVES!!!
//            rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
			rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum+_stateScalar*_slit)*(y-_maximum+_stateScalar*_slit)+(_slit*_maxflux)/(0.0243*_slit*_slit+0.415*_slit+3.4167);
		}
	}
	return rCurve;
}

double SGMResolutionOptimization::maximumEnergy(QList<AMXASRegion*> regions){
	double curMax = 240;
	if(regions.count() == 0)
		return -1;
	for(int x = 0; x < regions.count(); x++){
		curMax = (regions.at(x)->start() > curMax ? regions.at(x)->start() : curMax);
		curMax = (regions.at(x)->end() > curMax ? regions.at(x)->end() : curMax);
	}
	return curMax;
}

double SGMResolutionOptimization::minimumEnergy(QList<AMXASRegion*> regions){
	double curMin = 2000;
	if(regions.count() == 0)
		return -1;
	for(int x = 0; x < regions.count(); x++){
		curMin = (regions.at(x)->start() < curMin ? regions.at(x)->start() : curMin);
		curMin = (regions.at(x)->end() < curMin ? regions.at(x)->end() : curMin);
	}
	return curMin;
}
