#include "CLSAmptekSDD123DetectorNew.h"

CLSAmptekSDD123DetectorNew::CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMDetector(name, description, parent)
{
	baseName_ = baseName;

	units_ = "Counts";

	// This can be done in the constructor because it doesn't depend on anything.
	axes_ = QList<AMAxisInfo>();
	AMAxisInfo ai("Energy", 1024, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axes_ << ai;

	data_ = QVector<double>(1024, 0);

	allControls_ = new AMControlSet(this);
	startAcquisitionControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:startAcquisition", baseName+":spectrum:startAcquisition", QString(), this, 0.5);
	startAcquisitionControl_->setAllowsMovesWhileMoving(true);
	statusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:state", this);
	mcaChannelsControl_ = new AMReadOnlyPVControl(name+"MCAChannels", baseName+":parameters:MCAChannels", this);
	integrationTimeControl_ = new AMPVControl(name+"IntegrationTime", baseName+":parameters:PresetTime", baseName+":parameters:PresetTime", QString() , this, 0.05);
	detectorTemperatureControl_ = new AMReadOnlyPVControl(name+"DetectorTemperature", baseName+":parameters:DetectorTemperature", this);
	spectrumControl_ = new AMReadOnlyPVControl(name+"Spectrum", baseName+":spectrum", this);
	binnedSpectrumControl_ = new AMReadOnlyWaveformBinningPVControl(name+"BinnedSpectrum", baseName+":spectrum", 0, 1024, this);
	isRequestedControl_ = new AMPVControl(name+"IsRequested", baseName+":isRequested", baseName+":isRequested", QString(), this, 0.5);

	allControls_->addControl(startAcquisitionControl_);
	allControls_->addControl(statusControl_);
	allControls_->addControl(mcaChannelsControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(detectorTemperatureControl_);
	allControls_->addControl(spectrumControl_);
	allControls_->addControl(binnedSpectrumControl_);
	allControls_->addControl(isRequestedControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	connect(spectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlChanged()));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorTemperatureChanged(double)));

	spectrumDataSource_ = new AM1DProcessVariableDataSource(spectrumControl_->readPV(), "Spectrum", this);
}

double CLSAmptekSDD123DetectorNew::acquisitionTime() const{
	if(isConnected())
		return integrationTimeControl_->value();
	return -1;
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

const double* CLSAmptekSDD123DetectorNew::data() const{
	return data_.constData();
}

AMNumber CLSAmptekSDD123DetectorNew::reading(const AMnDIndex &indexes) const{
	if( (!isConnected()) || (indexes.rank() != 1) || (indexes.i() >= 1024) || indexes.i() < 0)
		return AMNumber(AMNumber::DimensionError);

	return spectrumControl_->readPV()->lastIntegerValues().at(indexes.i());
}

AMNumber CLSAmptekSDD123DetectorNew::singleReading() const{
	if(!isConnected())
		return AMNumber(AMNumber::Null);

	return binnedSpectrumControl_->value();

}

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

bool CLSAmptekSDD123DetectorNew::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if(!integrationTimeControl_->withinTolerance(seconds))
		integrationTimeControl_->move(seconds);
	return true;
}

bool CLSAmptekSDD123DetectorNew::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

bool CLSAmptekSDD123DetectorNew::initializeImplementation(){
	//setInitializing();
	setInitialized();
	return true;
}

bool CLSAmptekSDD123DetectorNew::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	AMControl::FailureExplanation failureExplanation = startAcquisitionControl_->move(1);
	if(failureExplanation != AMControl::NoFailure)
		return true;
	return false;
}

bool CLSAmptekSDD123DetectorNew::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void CLSAmptekSDD123DetectorNew::onControlsConnected(bool connected){
	if(connected)
		setConnected(connected);
	else
		setConnected(false);

	if(connected)
		setReadyForAcquisition();
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void CLSAmptekSDD123DetectorNew::onControlsTimedOut(){
	setConnected(false);
}

void CLSAmptekSDD123DetectorNew::onSpectrumControlChanged(){

	QVector<int> values = spectrumControl_->readPV()->lastIntegerValues();
	for(int x = 0, size = values.count(); x < size; x++)
		data_[x] = double(values.at(x));
}

void CLSAmptekSDD123DetectorNew::onStatusControlChanged(){

	if(statusControl_->withinTolerance(1))
		setAcquiring();
	else if(statusControl_->withinTolerance(0)){
		if(isAcquiring())
			setAcquisitionSucceeded();

		if(!isConnected() && !isNotReadyForAcquisition())
			setNotReadyForAcquisition();
		else if(isConnected() && !isReadyForAcquisition())
			setReadyForAcquisition();
	}
}
