#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : AMControl("SGMBeamline", "n/a") {
//	amNames2pvNames_.set("energy", "dave:Energy");
	amNames2pvNames_.set("energy", "reixsHost:Energy");
	amNames2pvNames_.set("eV_Fbk", "dave:Energy:fbk");
	amNames2pvNames_.set("mono", "dave:Energy:mono");
	amNames2pvNames_.set("undulator", "dave:Energy:undulator");
	amNames2pvNames_.set("exitSlit", "dave:Energy:exitSlit");
	amNames2pvNames_.set("exitSlitGap", "dave:Slit");
	amNames2pvNames_.set("M4", "dave:M4");
	amNames2pvNames_.set("M4Inboard", "dave:M4:inboard");
	amNames2pvNames_.set("M4Outboard", "dave:M4:outboard");
	amNames2pvNames_.set("M4Downstream", "dave:M4:downstream");
	amNames2pvNames_.set("grating", "dave:Energy:mono:grating");
	amNames2pvNames_.set("harmonic", "dave:Energy:undulator:harmonic");
	amNames2pvNames_.set("undulatorTracking", "dave:Energy:undulator:tracking");
	amNames2pvNames_.set("monoTracking", "dave:Energy:mono:tracking");
	amNames2pvNames_.set("exitSlitTracking", "dave:Energy:exitSlit:tracking");
//	amNames2pvNames_.set("tey", "dave:TEY");
	amNames2pvNames_.set("tey", "reixsHost:tey");
//	amNames2pvNames_.set("tfy", "dave:TFY");
	amNames2pvNames_.set("tfy", "reixsHost:tfy");
//	amNames2pvNames_.set("pgt", "dave:PGT");
	amNames2pvNames_.set("pgt", "reixsHost:sdd:spectrum");
	amNames2pvNames_.set("I0", "reixsHost:I0");

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::toPV("ringCurrent"), this);
	addChild(ringCurrent_);
	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.01);
	sgmPVName = amNames2pvNames_.valueF("mono");
	AMReadOnlyPVwStatusControl *mono = new AMReadOnlyPVwStatusControl("mono", sgmPVName, sgmPVName+":moving", energy_);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	AMReadOnlyPVwStatusControl *undulator = new AMReadOnlyPVwStatusControl("undulator", sgmPVName, sgmPVName+":moving", energy_);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	AMReadOnlyPVwStatusControl *exitSlit = new AMReadOnlyPVwStatusControl("exitSlit", sgmPVName, sgmPVName+":moving", energy_);
	energy_->addChild(mono);
	energy_->addChild(undulator);
	energy_->addChild(exitSlit);
	addChild(energy_);
	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	addChild(exitSlitGap_);
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName+":moving", this);
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	m4_->addChild(m4inboard);
	m4_->addChild(m4outboard);
	m4_->addChild(m4downstream);
	addChild(m4_);
	sgmPVName = amNames2pvNames_.valueF("grating");
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	addChild(grating_);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	addChild(harmonic_);
	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, this, 0.1);
	addChild(undulatorTracking_);
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, this, 0.1, 10);
	addChild(monoTracking_);
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, this, 0.1);
	addChild(exitSlitTracking_);
	sgmPVName = amNames2pvNames_.valueF("tey");
	tey_ = new AMReadOnlyPVControl("tey", sgmPVName, this);
	addChild(tey_);
	teyDetector_ = new AMSingleControlDetector(tey_->name(), tey_, this);
	sgmPVName = amNames2pvNames_.valueF("tfy");
	tfy_ = new AMReadOnlyPVControl("tfy", sgmPVName, this);
	addChild(tfy_);
	tfyDetector_ = new AMSingleControlDetector(tfy_->name(), tfy_, this);
	sgmPVName = amNames2pvNames_.valueF("pgt");
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);
	addChild(pgt_);
	pgtDetector_ = new AMSingleControlDetector(pgt_->name(), pgt_, this);

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

	XASDetectors_ = new AMDetectorSet(this);
	XASDetectors_->setName("XAS Detectors");
	XASDetectors_->addDetector(teyDetector_, true);
	XASDetectors_->addDetector(tfyDetector_, true);
	XASDetectors_->addDetector(pgtDetector_, false);

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

QMap<double, double> SGMFluxOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	qDebug() << "Running flux curve";
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
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);
		for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
			rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
		}
	}
	return rCurve;
}

double SGMFluxOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMFluxOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}


SGMResolutionOptimization::SGMResolutionOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMResolution";
}

QMap<double, double> SGMResolutionOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
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
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);
		for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
// FIX NEGATIVES!!!
//            rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
			rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum+_stateScalar*_slit)*(y-_maximum+_stateScalar*_slit)+(_slit*_maxflux)/(0.0243*_slit*_slit+0.415*_slit+3.4167);
		}
	}
	return rCurve;
}

double SGMResolutionOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMResolutionOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}
