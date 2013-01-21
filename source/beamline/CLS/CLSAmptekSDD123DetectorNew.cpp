#include "CLSAmptekSDD123DetectorNew.h"

CLSAmptekSDD123DetectorNew::CLSAmptekSDD123DetectorNew(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMDetector(name, description, parent)
{
	baseName_ = baseName;

	units_ = "Counts";

	data_ = new double[1024];
	for(int x = 0; x < 1024; x++)
		data_[x] = 0;

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

	connect(spectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlChanged(double)));

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);
}

int CLSAmptekSDD123DetectorNew::size(int axisNumber) const{
	if(axisNumber = 0)
		return 1024;
	return -1;
}

QList<AMAxisInfo> CLSAmptekSDD123DetectorNew::axes() const{
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", 1024, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;
	return axisInfo;
}

bool CLSAmptekSDD123DetectorNew::isAcquiring() const{
	if(isConnected())
		return statusControl_->withinTolerance(1);
	return false;
}

double CLSAmptekSDD123DetectorNew::acquisitionTime() const{
	if(isConnected())
		return integrationTimeControl_->value();
	return -1;
}

bool CLSAmptekSDD123DetectorNew::currentlySynchronizedDwell() const{
	// FIGURE OUT HOW TO DO THIS
	return false;
}

const double* CLSAmptekSDD123DetectorNew::data() const{
	return data_;
}

AMNumber CLSAmptekSDD123DetectorNew::reading(const AMnDIndex &indexes) const{
	if( (!isConnected()) || (indexes.rank() != 1) || (indexes.i() > 1024) )
		return AMNumber(AMNumber::DimensionError);

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	return tmpControl->readPV()->lastIntegerValues().at(indexes.i());
}

bool CLSAmptekSDD123DetectorNew::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if(!integrationTimeControl_->withinTolerance(seconds))
		integrationTimeControl_->move(seconds);
	return true;
}

bool CLSAmptekSDD123DetectorNew::acquire(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	startAcquisitionControl_->move(1);
	return true;
}

void CLSAmptekSDD123DetectorNew::onControlsConnected(bool connected){
	setConnected(connected);
}

void CLSAmptekSDD123DetectorNew::onControlsTimedOut(){
	setConnected(false);
}

void CLSAmptekSDD123DetectorNew::onSpectrumControlChanged(double newValue){
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);

	QVector<int> values = tmpControl->readPV()->lastIntegerValues();
	for(int x = 0; x < values.count(); x++)
		data_[x] = values.at(x);
}
