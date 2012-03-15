#include "CLSAmptekSDD123Detector.h"

CLSAmptekSDD123Detector::CLSAmptekSDD123Detector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod, QObject *parent) :
	CLSAmptekSDD123DetectorInfo(name, name, parent), AMDetector(name, readMethod)
{
	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	setElements(1);
	wasConnected_ = false;

	allControls_ = new AMControlSet(this);
	startAcquisitionControl_ = new AMPVControl(name+"StartAcquisition", baseName+":spectrum:startAcquisition", baseName+":spectrum:startAcquisition", QString(), this, 0.5);
	statusControl_ = new AMReadOnlyPVControl(name+"Status", baseName+":spectrum:state", this);
	mcaChannelsControl_ = new AMReadOnlyPVControl(name+"MCAChannels", baseName+":parameters:MCAChannels", this);
	integrationTimeControl_ = new AMReadOnlyPVControl(name+"IntegrationTime", baseName+":parameters:PresetTime", this);
	detectorTemperatureControl_ = new AMReadOnlyPVControl(name+"DetectorTemperature", baseName+":parameters:DetectorTemperature", this);
	spectrumControl_ = new AMReadOnlyPVControl(name+"Spectrum", baseName+":spectrum", this);

	allControls_->addControl(startAcquisitionControl_);
	allControls_->addControl(statusControl_);
	allControls_->addControl(mcaChannelsControl_);
	allControls_->addControl(integrationTimeControl_);
	allControls_->addControl(detectorTemperatureControl_);
	allControls_->addControl(spectrumControl_);
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));

	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	spectrumDataSource_ = new AM1DProcessVariableDataSource(tmpControl->readPV(), "Spectrum", this);

	connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusChanged(double)));
	connect(mcaChannelsControl_, SIGNAL(valueChanged(double)), this, SLOT(setChannelsAsDouble(double)));
	connect(mcaChannelsControl_, SIGNAL(valueChanged(double)), this, SIGNAL(mcaChannelsChanged(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setIntegrationTime(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(integrationTimeChanged(double)));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SLOT(setDetectorTemperature(double)));
	connect(detectorTemperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorTemperatureChanged(double)));
}

CLSAmptekSDD123Detector::~CLSAmptekSDD123Detector()
{

}

const QMetaObject* CLSAmptekSDD123Detector::getMetaObject(){
	return metaObject();
}

QString CLSAmptekSDD123Detector::dacqName() const{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	if(isConnected() && tmpControl)
		return tmpControl->readPVName();
	else
		return "";
}

QString CLSAmptekSDD123Detector::description() const{
	return CLSAmptekSDD123DetectorInfo::description();
}

AMDetectorInfo* CLSAmptekSDD123Detector::toInfo() const{
	return new CLSAmptekSDD123DetectorInfo(*this);
}

CLSAmptekSDD123DetectorInfo CLSAmptekSDD123Detector::toCLSAmptekSDD123Info() const{
	return CLSAmptekSDD123DetectorInfo(*this);
}

bool CLSAmptekSDD123Detector::setFromInfo(const AMDetectorInfo *info)
{
	const CLSAmptekSDD123DetectorInfo *detectorInfo = qobject_cast<const CLSAmptekSDD123DetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	return true;
}

void CLSAmptekSDD123Detector::fromCLSAmptekSDD123Info(const CLSAmptekSDD123DetectorInfo &info)
{
	Q_UNUSED(info)
}

bool CLSAmptekSDD123Detector::wasConnected() const{
	return wasConnected_;
}

bool CLSAmptekSDD123Detector::status() const{
	if(statusControl_->withinTolerance(1))
		return true;
	return false;
}

QVector<int> CLSAmptekSDD123Detector::spectraValues()
{
	AMReadOnlyPVControl *tmpControl = qobject_cast<AMReadOnlyPVControl*>(spectrumControl_);
	return tmpControl->readPV()->lastIntegerValues();
}

AMDataSource* CLSAmptekSDD123Detector::spectrumDataSource() const{
	return spectrumDataSource_;
}

QDebug CLSAmptekSDD123Detector::qDebugPrint(QDebug &d) const{
	d << "CLSAmptekSDD123Detector " << name();
	return d;
}

void CLSAmptekSDD123Detector::start(){
	startAcquisitionControl_->move(1);
}

void CLSAmptekSDD123Detector::setDescription(const QString &description){
	CLSAmptekSDD123DetectorInfo::setDescription(description);
}

void CLSAmptekSDD123Detector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void CLSAmptekSDD123Detector::onStatusChanged(double status){
	Q_UNUSED(status)
	if(statusControl_->withinTolerance(1))
		emit statusChanged(true);
	else
		emit statusChanged(false);
}
