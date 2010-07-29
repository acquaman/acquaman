#include "SGMBeamline.h"

SGMBeamline* SGMBeamline::instance_ = 0;


SGMBeamline::SGMBeamline() : AMControl("SGMBeamline", "n/a") {
//	amNames2pvNames_.set("energy", "dave:Energy");
	amNames2pvNames_.set("energy", "reixsHost:Energy");
	amNames2pvNames_.set("eV_Fbk", "dave:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "dave:Energy:fbk");
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
	amNames2pvNames_.set("loadlockCCG", "dave:Endstation:loadlock:ccg");
	amNames2pvNames_.set("loadlockTCG", "dave:Endstation:loadlock:tcg");

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
	gratingAction_ = new SGMGratingAction(grating_, this);
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
	QStringList pgtYElements;
	pgtYElements << "pgtCounts";
	pgtDetector_ = new AMSpectralOutputDetector(pgt_->name(), pgt_, 1024, "pgtEV", pgtYElements, this);
	sgmPVName = amNames2pvNames_.valueF("I0");
	i0_ = new AMReadOnlyPVControl("I0", sgmPVName, this);
	addChild(i0_);
	i0Detector_ = new AMSingleControlDetector(i0_->name(), i0_, this);
	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);
	addChild(eVFbk_);
	eVFbkDetector_ = new AMSingleControlDetector(eVFbk_->name(), eVFbk_, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);

	fluxOptimization_ = new SGMFluxOptimization(this);
	fluxOptimization_->setDescription("Flux");
	resolutionOptimization_ = new SGMResolutionOptimization(this);
	resolutionOptimization_->setDescription("Resolution");
	fluxResolutionSet_ = new AMControlOptimizationSet(this);
//	fluxResolutionSet_->setName("Flux and Resolution");
	fluxResolutionSet_->setName("Flux/Resolution");
	fluxResolutionSet_->addControl(grating_);
	fluxResolutionSet_->addControl(harmonic_);
	fluxResolutionSet_->addControl(exitSlitGap_);
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(fluxOptimization_);
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(resolutionOptimization_);

	trackingSet_ = new AMControlSet(this);
	trackingSet_->setName("Tracking");
	trackingSet_->addControl(undulatorTracking_);
	trackingSet_->addControl(monoTracking_);
	trackingSet_->addControl(exitSlitTracking_);

	allDetectors_ = new AMDetectorInfoSet(this);
	allDetectors_->setName("All Detectors");
	allDetectors_->addDetector(i0Detector_, true);
	allDetectors_->addDetector(eVFbkDetector_, true);
	allDetectors_->addDetector(teyDetector_, true);
	allDetectors_->addDetector(tfyDetector_, true);
	allDetectors_->addDetector(pgtDetector_, false);

	XASDetectors_ = new AMDetectorInfoSet(this);
	XASDetectors_->setName("XAS Detectors");
	XASDetectors_->addDetector(teyDetector_, true);
	XASDetectors_->addDetector(tfyDetector_, true);
	XASDetectors_->addDetector(pgtDetector_, false);

	transferAction1_ = new SGMTransferAction1(this);
	transferAction2_ = new SGMTransferAction2(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction2_, SLOT(checkValue(double)));
	transferAction3_ = new SGMTransferAction3(this);
	transferAction4_ = new SGMTransferAction4(this);
	transferAction5_ = new SGMTransferAction5(this);
	connect(loadlockTCG_, SIGNAL(valueChanged(double)), transferAction5_, SLOT(checkValue(double)));
	transferAction6_ = new SGMTransferAction6(this);

	transferAction7_ = new SGMTransferAction7(this);
	transferAction8_ = new SGMTransferAction8(this);
	transferAction9_ = new SGMTransferAction9(this);
	connect(loadlockTCG_, SIGNAL(valueChanged(double)), transferAction9_, SLOT(checkValue(double)));
	transferAction10_ = new SGMTransferAction10(this);
	transferAction11_ = new SGMTransferAction11(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction11_, SLOT(checkValue(double)));
	transferAction12_ = new SGMTransferAction12(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction12_, SLOT(checkValue(double)));

	transferAction13_ = new SGMTransferAction13(this);
	transferAction14_ = new SGMTransferAction14(this);
	transferAction15_ = new SGMTransferAction15(this);
	transferAction16_ = new SGMTransferAction16(this);
	transferAction17_ = new SGMTransferAction17(this);
	transferAction18_ = new SGMTransferAction18(this);
	transferAction19_ = new SGMTransferAction19(this);
	transferAction20_ = new SGMTransferAction20(this);
	transferAction21_ = new SGMTransferAction21(this);
	transferAction22_ = new SGMTransferAction22(this);
	transferAction23_ = new SGMTransferAction23(this);
	transferAction24_ = new SGMTransferAction24(this);
	transferAction25_ = new SGMTransferAction25(this);

	transferAction1_->setNext(transferAction2_);
	transferAction2_->setPrevious(transferAction1_);
	transferAction2_->setNext(transferAction3_);
	transferAction3_->setPrevious(transferAction2_);
	transferAction3_->setNext(transferAction4_);
	transferAction4_->setPrevious(transferAction3_);
	transferAction4_->setNext(transferAction5_);
	transferAction5_->setPrevious(transferAction4_);
	transferAction5_->setNext(transferAction6_);
	transferAction6_->setPrevious(transferAction5_);

	transferAction7_->setNext(transferAction8_);
	transferAction8_->setPrevious(transferAction7_);
	transferAction8_->setNext(transferAction9_);
	transferAction9_->setPrevious(transferAction8_);
	transferAction9_->setNext(transferAction10_);
	transferAction10_->setPrevious(transferAction9_);
	transferAction10_->setNext(transferAction11_);
	transferAction11_->setPrevious(transferAction10_);
	transferAction11_->setNext(transferAction12_);
	transferAction12_->setPrevious(transferAction11_);

	transferAction13_->setNext(transferAction14_);
	transferAction14_->setPrevious(transferAction13_);
	transferAction14_->setNext(transferAction15_);
	transferAction15_->setPrevious(transferAction14_);
	transferAction15_->setNext(transferAction16_);
	transferAction16_->setPrevious(transferAction15_);
	transferAction16_->setNext(transferAction17_);
	transferAction17_->setPrevious(transferAction16_);
	transferAction17_->setNext(transferAction18_);
	transferAction18_->setPrevious(transferAction17_);
	transferAction18_->setNext(transferAction19_);
	transferAction19_->setPrevious(transferAction18_);
	transferAction19_->setNext(transferAction20_);
	transferAction20_->setPrevious(transferAction19_);

	transferAction21_->setNext(transferAction22_);
	transferAction22_->setPrevious(transferAction21_);
	transferAction22_->setNext(transferAction23_);
	transferAction23_->setPrevious(transferAction22_);
	transferAction23_->setNext(transferAction24_);
	transferAction24_->setPrevious(transferAction23_);
	transferAction24_->setNext(transferAction25_);
	transferAction25_->setPrevious(transferAction24_);
}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy){
	if( (grating == 0) && (harmonic == 1) && (energy > 240) && (energy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == 1) && (energy > 440) && (energy < 1200) )
		return true;
	//else if( (grating == 2) && (harmonic == 1) && (energy > 800) && (energy < 1100) )
	else if( (grating == 2) && (harmonic == 1) && (energy > 800) && (energy < 1150) )
		return true;
	//else if( (grating == 2) && (harmonic == 3) && (energy > 1100) && (energy < 2000) )
	else if( (grating == 2) && (harmonic == 3) && (energy > 1050) && (energy < 2000) )
		return true;
	else
		return false;
}

bool SGMBeamline::energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy){
	if( (grating == 0) && (harmonic == 1) && (maxEnergy > 240) && (minEnergy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == 1) && (maxEnergy > 440) && (minEnergy < 1200) )
		return true;
	//else if( (grating == 2) && (harmonic == 1) && (maxEnergy > 800) && (minEnergy < 1100) )
	else if( (grating == 2) && (harmonic == 1) && (maxEnergy > 800) && (minEnergy < 1150) )
		return true;
	//else if( (grating == 2) && (harmonic == 3) && (maxEnergy > 1100) && (minEnergy < 2000) )
	else if( (grating == 2) && (harmonic == 3) && (maxEnergy > 1050) && (minEnergy < 2000) )
		return true;
	else
		return false;
}

QList< QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> > SGMBeamline::gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy){
	QList< QPair<sgmGrating, sgmHarmonic> > rVal;
	if( (maxEnergy > 240) && (maxEnergy < 750) && (minEnergy > 240) && (minEnergy < 750) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)0, (SGMBeamline::sgmHarmonic)1 ));
	if((maxEnergy > 440) && (maxEnergy < 1200) && (minEnergy > 440) && (minEnergy < 1200) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)1, (SGMBeamline::sgmHarmonic)1 ));
	if( (maxEnergy > 800) && (maxEnergy < 1100) && (minEnergy > 800) && (minEnergy < 1100) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, (SGMBeamline::sgmHarmonic)1 ));
	if( (maxEnergy > 1100) && (maxEnergy < 2000) && (minEnergy > 1100) && (minEnergy < 2000) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, (SGMBeamline::sgmHarmonic)3 ));
	return rVal;
}

QPair<double, double> SGMBeamline::energyRangeForGratingHarmonic(SGMBeamline::sgmGrating grating, SGMBeamline::sgmHarmonic harmonic){
	QPair<double, double> rVal;
	if( (grating == 0) && (harmonic == 1) )
		rVal = QPair<double, double>(240, 750);
	else if( (grating == 1) && (harmonic == 1) )
		rVal = QPair<double, double>(440, 1200);
	else if( (grating == 2) && (harmonic == 1) )
		rVal = QPair<double, double>(800, 1100);
	else if( (grating == 2) && (harmonic == 3) )
		rVal = QPair<double, double>(1100, 2000);
	return rVal;
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

QMap< QString, QMap<double, double> > SGMFluxOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	//int numPoints = 100;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> fluxL1, fluxM1, fluxH1, fluxH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		fluxL1.insert(x, collapser(curve(l1, contextParameters)));
		fluxM1.insert(x, collapser(curve(m1, contextParameters)));
		fluxH1.insert(x, collapser(curve(h1, contextParameters)));
		fluxH3.insert(x, collapser(curve(h3, contextParameters)));
		qDebug() << x << " collapses to " << fluxL1.value(x) << fluxM1.value(x) << fluxH1.value(x) << fluxH3.value(x);
	}

	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", fluxL1);
	rVal.insert("MEG1", fluxM1);
	rVal.insert("HEG1", fluxH1);
	rVal.insert("HEG3", fluxH3);
	return rVal;
}

double SGMFluxOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
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
	double _y1, _y2, _y3, _x1, _x2, _x3;
	SGMBeamline::sgmGrating _grating = (SGMBeamline::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamline::sgmHarmonic _harmonic = (SGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == 3) && (_grating == 2)){
		_y1 = (0.95)*(0.5229*log(_slit)+1.4221);
		_y2 = (0.95)*(0.4391*log(_slit)+1.2617);
		_y3 = (0.95)*(0.421*log(_slit)+0.9037);
		_x1 = 2000;
		_x2 = 1200;
		_x3 = 800;
	}
	else{
		if( (_grating == 0) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.4568*log(_slit)+1.0199;
			_y2 = 0.4739*log(_slit)+0.605;
			_y3 = 0.4257*log(_slit)+0.4438;
			_x1 = 600;
			_x2 = 400;
			_x3 = 250;
		}
		else if( (_grating == 1) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.425*log(_slit)+1.4936;
			_y2 = 0.4484*log(_slit)+1.0287;
			_y3 = 0.4029*log(_slit)+0.7914;
			_x1 = 1200;
			_x2 = 800;
			_x3 = 500;
		}
		else if( (_grating == 2) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.5229*log(_slit)+1.4221;
			_y2 = 0.4391*log(_slit)+1.2617;
			_y3 = 0.421*log(_slit)+0.9037;
			_x1 = 2000;
			_x2 = 1200;
			_x3 = 800;
		}
	}
	int i, n;
	double xi, yi, ei, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;

	n = 3;

	X = gsl_matrix_alloc (n, 3);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (3);
	cov = gsl_matrix_alloc (3, 3);

	double ix[3];
	double iy[3];
	double ie[3];
	ix[0] = _x1;
	ix[1] = _x2;
	ix[2] = _x3;
	iy[0] = _y1;
	iy[1] = _y2;
	iy[2] = _y3;
	ie[0] = 0.1*iy[0];
	ie[1] = 0.1*iy[1];
	ie[2] = 0.1*iy[2];
	for (i = 0; i < n; i++)
	{
		xi = ix[i];
		yi = iy[i];
		ei = ie[i];

		gsl_matrix_set (X, i, 0, 1.0);
		gsl_matrix_set (X, i, 1, xi);
		gsl_matrix_set (X, i, 2, xi*xi);

		gsl_vector_set (y, i, yi);
		gsl_vector_set (w, i, 1.0/(ei*ei));
	}

	gsl_multifit_linear_workspace * work
			= gsl_multifit_linear_alloc (n, 3);
	gsl_multifit_wlinear (X, w, y, c, cov,
						  &chisq, work);
	gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta, tmpVal;
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);

		for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
			rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 :y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
		}
	}
	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);
	return rCurve;
}

QMap< QString, QMap<double, double> > SGMResolutionOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	//int numPoints = 100;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> resL1, resM1, resH1, resH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		resL1.insert(x, collapser(curve(l1, contextParameters)));
		resM1.insert(x, collapser(curve(m1, contextParameters)));
		resH1.insert(x, collapser(curve(h1, contextParameters)));
		resH3.insert(x, collapser(curve(h3, contextParameters)));
		qDebug() << x << " collapses to " << resL1.value(x) << resM1.value(x) << resH1.value(x) << resH3.value(x);
	}
	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", resL1);
	rVal.insert("MEG1", resM1);
	rVal.insert("HEG1", resH1);
	rVal.insert("HEG3", resH3);
	return rVal;
}

double SGMResolutionOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
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

SGMGratingAction::SGMGratingAction(AMControl* grating, QObject *parent) :
		AMBeamlineControlAction(grating, "", parent)
{
}

void SGMGratingAction::start(){
	((AMPVwStatusControl*)control_)->move(1);
}
