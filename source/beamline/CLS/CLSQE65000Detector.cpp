#include "CLSQE65000Detector.h"

#include <QStringBuilder>

CLSQE65000Detector::CLSQE65000Detector(const QString &name, const QString &description, const QString &baseName, QObject *parent) :
	AMDetector(name, description, parent)
{
	baseName_ = baseName;
	units_ = "Counts";

	data_ = new double[1024];
	for(int x = 0; x < 1024; x++)
		data_[x] = 0;

	allControls_ = new AMControlSet(this);

	spectrumControl_ = new AMReadOnlyPVControl(name%"Spectrum", baseName%":DarkCorrectedSpectra", this);
	spectrumControl_->setDescription("QE 65000 Spectrum");
	spectrumControl_->setContextKnownDescription("Spectrum");
	binnedSpectrumControl_ = new AMReadOnlyWaveformBinningPVControl(name%"BinnedSpectrum", baseName%":DarkCorrectedSpectra", 0, 1024, this);
	((AMReadOnlyWaveformBinningPVControl*)binnedSpectrumControl_)->setAttemptDouble(true);
	integrationTimeControl_ = new AMPVControl(name%"IntegrationTime", baseName%":IntegrationTime:Value", baseName%":IntegrationTime:Value", "", this, 0.1);
	integrationTimeControl_->setDescription("QE 65000 Integration Time");
	integrationTimeControl_->setContextKnownDescription("Integration Time");

	startAcquisitionControl_ = new AMPVControl(name%"StartAcquisition", baseName%":Acquire", baseName%":Acquire", "", this, 0.1);
	startAcquisitionControl_->setDescription("QE 65000 Start Acquisition");
	startAcquisitionControl_->setContextKnownDescription("Start Acquisition");
	statusControl_ = new AMReadOnlyPVControl(name+"DwellStatus", baseName+":Acquiring", this);
	statusControl_->setDescription("QE 65000 Status");
	statusControl_->setContextKnownDescription("Status");

	allControls_->addControl(spectrumControl_);
	allControls_->addControl(binnedSpectrumControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(startAcquisitionControl_);
	allControls_->addControl(statusControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	connect(spectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlChanged(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged(double)));

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);
}

int CLSQE65000Detector::size(int axisNumber) const{
	if(axisNumber == 0)
		return 1024;
	return -1;
}

QList<AMAxisInfo> CLSQE65000Detector::axes() const{
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Wavelength", 1024, "Wavelength", "nm");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;
	return axisInfo;
}

double CLSQE65000Detector::acquisitionTime() const{
	if(isConnected())
		return integrationTimeControl_->value();
	return -1;
}

QString CLSQE65000Detector::synchronizedDwellKey() const{
	return QString("%1:Acquire NPP NMS").arg(baseName_);
}

bool CLSQE65000Detector::sharesDetectorTriggerSource(){
	return currentlySynchronizedDwell();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSQE65000Detector::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return 0;
}

AMDetectorDwellTimeSource* CLSQE65000Detector::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return 0;
}

AMNumber CLSQE65000Detector::reading(const AMnDIndex &indexes) const{
	if( (!isConnected()) || (indexes.rank() != 1) || (indexes.i() > 1024) )
		return AMNumber(AMNumber::DimensionError);

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	return tmpControl->readPV()->lastIntegerValues().at(indexes.i());
}

AMNumber CLSQE65000Detector::singleReading() const{
	if(!isConnected())
		return AMNumber(AMNumber::Null);

	AMReadOnlyWaveformBinningPVControl *tmpControl = qobject_cast<AMReadOnlyWaveformBinningPVControl*>(binnedSpectrumControl_);
	return tmpControl->value();

}

bool CLSQE65000Detector::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues)

	return false;
}

const double* CLSQE65000Detector::data() const{
	return data_;
}

bool CLSQE65000Detector::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if(!integrationTimeControl_->withinTolerance(seconds))
		integrationTimeControl_->move(seconds);
	return true;
}

bool CLSQE65000Detector::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

bool CLSQE65000Detector::initializeImplementation(){
	//setInitializing();
	setInitialized();
	return true;
}

bool CLSQE65000Detector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	AMControl::FailureExplanation failureExplanation = startAcquisitionControl_->move(1);
	if(failureExplanation != AMControl::NoFailure)
		return true;
	return false;
}

bool CLSQE65000Detector::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void CLSQE65000Detector::onControlsConnected(bool connected){
	if(connected)
		setConnected(connected);
	else
		setConnected(false);

	if(connected)
		setReadyForAcquisition();
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void CLSQE65000Detector::onControlsTimedOut(){
	setConnected(false);
}

void CLSQE65000Detector::onSpectrumControlChanged(double newValue){
	Q_UNUSED(newValue)

	if(isConnected()){
		AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);

		memcpy(data_, tmpControl->readPV()->lastFloatingPointValues().constData(), 1024*sizeof(double));
		/*
		QVector<int> values = tmpControl->readPV()->lastIntegerValues();
		//QVector<double> values = tmpControl->readPV()->lastFloatingPointValues();
		for(int x = 0; x < values.count(); x++)
			data_[x] = values.at(x);
		*/
	}
}

void CLSQE65000Detector::onStatusControlChanged(double value){
	Q_UNUSED(value)

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
