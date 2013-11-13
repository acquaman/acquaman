#include "CLSAmptekSDD123DetectorNew.h"

CLSAmptekSDD123DetectorNew::CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMXRFDetector(name, description, parent)
{	
	baseName_ = baseName;

	units_ = "Counts";

	// This can be done in the constructor because it doesn't depend on anything.
	axes_ = QList<AMAxisInfo>();
	AMAxisInfo ai("Energy", 1024, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	ai.increment = 10.4;
	axes_ << ai;

	//data_ = QVector<double>(1024, 0);

	//acquireControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:startAcquisition", baseName+":spectrum:startAcquisition", QString(), this, 0.5);
	acquireControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:start", baseName+":spectrum:start", QString(), this, 0.5);
	acquireControl_->setAllowsMovesWhileMoving(true);
	//acquisitionStatusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:state", this);
	acquisitionStatusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:dwellState", this);
	//acquireTimeControl_ = new AMPVControl(name+"IntegrationTime", baseName+":parameters:PresetTime", baseName+":parameters:PresetTime", QString() , this, 0.05);
	acquireTimeControl_ = new AMPVControl(name+"IntegrationTime", baseName+":spectrum:dwellTime", baseName+":spectrum:dwellTime", QString() , this, 0.05);

	icrControls_.append(new AMReadOnlyPVControl("Input Counts", baseName+":spectrum:fastCounts", this, "Input counts for Amptek"));
	ocrControls_.append(new AMReadOnlyPVControl("Output Counts", baseName+":spectrum:slowCounts", this, "Output counts for Amptek"));
	spectraControls_.append(new AMReadOnlyPVControl("Raw Spectrum", baseName+":spectrum", this));

	mcaChannelsControl_ = new AMReadOnlyPVControl(name+"MCAChannels", baseName+":parameters:MCAChannels", this);
	detectorTemperatureControl_ = new AMReadOnlyPVControl(name+"DetectorTemperature", baseName+":parameters:DetectorTemperature", this);
	binnedSpectrumControl_ = new AMReadOnlyWaveformBinningPVControl(name+"BinnedSpectrum", baseName+":spectrum", 0, 1024, this);
	isRequestedControl_ = new AMPVControl(name+"IsRequested", baseName+":isRequested", baseName+":isRequested", QString(), this, 0.5);
	//AMReadOnlyPVControl *spectrumControl = new AMReadOnlyPVControl(name+"Spectrum", baseName+":spectrum", this);

	//spectraControls_.append(spectrumControl);

	/*
	allControls_->addControl(mcaChannelsControl_);
	allControls_->addControl(detectorTemperatureControl_);
	allControls_->addControl(binnedSpectrumControl_);
	allControls_->addControl(isRequestedControl_);
	*/

	//connect(spectrumControl, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlChanged()));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorTemperatureChanged(double)));

	allControlsCreated();	// This is very important.  It does a lot of work for you building data sources and the like.

	((AM1DProcessVariableDataSource *)rawSpectraSources_.first())->setScale(ai.increment);
}

QString CLSAmptekSDD123DetectorNew::synchronizedDwellKey() const{
	return "amptek:sdd:all:spectrum:startAcquisitio";
}

bool CLSAmptekSDD123DetectorNew::sharesDetectorTriggerSource(){
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSAmptekSDD123DetectorNew::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return 0;
}

AMDetectorDwellTimeSource* CLSAmptekSDD123DetectorNew::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return 0;
}

//const double* CLSAmptekSDD123DetectorNew::data() const{
//	return data_.constData();
//}

//AMNumber CLSAmptekSDD123DetectorNew::singleReading() const{
//	/**/
//	if(!isConnected())
//		return AMNumber(AMNumber::Null);

////<<<<<<< HEAD
//	return binnedSpectrumControl_->value();
////=======
////	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(binnedSpectrumControl_);
////	return tmpControl->value();
//	/**/

////	if(!isConnected())
////		return AMNumber(AMNumber::Null);
////	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
////	QVector<int> lastIntegerValues = tmpControl->readPV()->lastIntegerValues();
////	int retVal = 0;
////	for(int x = 0; x < 1024; x++)
////		retVal += lastIntegerValues.at(x);
////	return retVal;
////>>>>>>> master
//}

bool CLSAmptekSDD123DetectorNew::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues)

	return false;
}

#include "actions3/actions/AMControlMoveAction3.h"
AMAction3* CLSAmptekSDD123DetectorNew::createEnableAction3(bool setEnabled){
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = isRequestedControl_->toInfo();
	setpoint.setValue(setEnabled ? 1 : 0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, isRequestedControl_);

	if(!action)
		return 0; //NULL

	return action;
}

bool CLSAmptekSDD123DetectorNew::isEnabled() const{
	if(!isConnected())
		return false;

	return isRequestedControl_->withinTolerance(1);
}

double CLSAmptekSDD123DetectorNew::detectorTemperature() const{
	if(!isConnected())
		return -1;
	return detectorTemperatureControl_->value();
}

bool CLSAmptekSDD123DetectorNew::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

void CLSAmptekSDD123DetectorNew::onSpectrumControlChanged(){

	//dataSource()->values(0, data_.size()-1, data_.data());
}

/*
void CLSAmptekSDD123DetectorNew::onStatusControlChangedHelper(){
	setAcquisitionSucceeded();

	if(!isConnected() && !isNotReadyForAcquisition())
		setNotReadyForAcquisition();
	else if(isConnected() && !isReadyForAcquisition())
		setReadyForAcquisition();
}
*/
