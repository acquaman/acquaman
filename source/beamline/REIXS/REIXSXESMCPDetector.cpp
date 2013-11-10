#include "REIXSXESMCPDetector.h"

#include <QTimer>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "util/AMErrorMonitor.h"
#include "beamline/AMDetectorTriggerSource.h"

REIXSXESMCPDetector::REIXSXESMCPDetector(QObject *parent) :
	AMDetector("XESMCPDetector", "REIXS XES MCP Detector", parent)
{
	dwellTime_ = -1;
	totalCountTarget_ = -1;
	finishedConditions_ = REIXSXESMCPDetector::FinishedTotalTime;
	clearOnStart_ = true;
	dwellTimeTimer_ = 0; //NULL

	basePVName_ = "CPD1610-01";

	totalCountsControl_ = new AMReadOnlyPVControl("MCPTotalCounts", QString("%1:totalCounts").arg(basePVName_), this);
	countsPerSecondControl_ = new AMReadOnlyPVControl("MCPCountsPerSecond", QString("%1:countsPerSecond").arg(basePVName_), this);
	imageControl_ = new AMReadOnlyPVControl("MCPImage", QString("%1:image").arg(basePVName_), this);
	instantaneousImageControl_ = new AMReadOnlyPVControl("MCPInstantaneousImage", QString("%1:instantaneousImage").arg(basePVName_), this);
	resolutionXControl_ = new AMReadOnlyPVControl("MCPXResolution", QString("%1:resolutionX").arg(basePVName_), this);
	resolutionYControl_ = new AMReadOnlyPVControl("MCPYResolution", QString("%1:resolutionY").arg(basePVName_), this);

	clearControl_ = new AMSinglePVControl("MCPClear", QString("%1:clear").arg(basePVName_), this, 0.5);
	averagingPeriodSecsControl_ = new AMSinglePVControl("mcpAveragingPeriod", QString("%1:averagingPeriodSecs").arg(basePVName_), this, 0.1);
	persistTimeSecsControl_ = new AMSinglePVControl("mcpPersistDuration", QString("%1:persistTimeSecs").arg(basePVName_), this, 0.1);

	image_ = new REIXSXESMCPDataSource("xesImage", imageControl_, resolutionXControl_, resolutionYControl_, this);
	image_->setDescription("Accumulated Detector Image");
	instantaneousImage_ = new REIXSXESMCPDataSource("xesRealtimeImage", instantaneousImageControl_, resolutionXControl_, resolutionYControl_, this);
	instantaneousImage_->setDescription("Instantaneous Detector Image");

	allControls_ = new AMControlSet(this);
	allControls_->addControl(totalCountsControl_);
	allControls_->addControl(countsPerSecondControl_);
	allControls_->addControl(imageControl_);
	allControls_->addControl(instantaneousImageControl_);
	allControls_->addControl(resolutionXControl_);
	allControls_->addControl(resolutionYControl_);

	allControls_->addControl(clearControl_);
	allControls_->addControl(averagingPeriodSecsControl_);
	allControls_->addControl(persistTimeSecsControl_);

	triggerSource_ = new AMDetectorTriggerSource(QString("REIXSXESMCPTriggerSource"), this);
	connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	dwellTimeSource_ = new AMDetectorDwellTimeSource(QString("REIXSXESMCPDwellTimeSource"), this);
	connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellTimeSourceSetDwellTime(double)));

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(onControlsTimedOut()));
}

double REIXSXESMCPDetector::acquisitionTime() const{
	return dwellTime_;
}

AMDetectorTriggerSource* REIXSXESMCPDetector::detectorTriggerSource(){
	return triggerSource_;
}

AMDetectorDwellTimeSource* REIXSXESMCPDetector::detectorDwellTimeSource(){
	return dwellTimeSource_;
}

AMNumber REIXSXESMCPDetector::reading(const AMnDIndex& indexes) const{
	return image_->value(indexes);
}

AMNumber REIXSXESMCPDetector::singleReading() const{
	return totalCountsControl_->value();
}

bool REIXSXESMCPDetector::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues)
	return false;
}

const double* REIXSXESMCPDetector::data() const{
	QVector<int> intVal = imageControl_->readPV()->lastIntegerValues();
	QVector<double> retVal(intVal.size());
	for(int x = 0, size = intVal.size(); x < size; x++)
		retVal[x] = intVal.at(x);
	return retVal.constData();
}

double REIXSXESMCPDetector::totalCounts() const{
	return totalCountsControl_->value();
}

double REIXSXESMCPDetector::countsPerSecond() const{
	return countsPerSecondControl_->value();
}

REIXSXESMCPDetector::XESMCPFinishedConditions REIXSXESMCPDetector::finishedConditions() const{
	return finishedConditions_;
}

bool REIXSXESMCPDetector::clearOnStart() const{
	return clearOnStart_;
}

int REIXSXESMCPDetector::totalCountTarget() const{
	return totalCountTarget_;
}

void REIXSXESMCPDetector::setFinishedConditionTotalTime(){
	setFinishedCondition(REIXSXESMCPDetector::FinishedTotalTime);
}

void REIXSXESMCPDetector::setFinishedConditionTotalCounts(){
	setFinishedCondition(REIXSXESMCPDetector::FinishedTotalCounts);
}

void REIXSXESMCPDetector::setFinishedConditionTotalTimeOrTotalCounts(){
	setFinishedCondition(REIXSXESMCPDetector::FinishedTotalTime | REIXSXESMCPDetector::FinishedTotalCounts);
}

void REIXSXESMCPDetector::setClearOnStart(bool clearOnStart){
	if(clearOnStart_ != clearOnStart)
		clearOnStart_ = clearOnStart;
}

void REIXSXESMCPDetector::setTotalCountTarget(int totalCountTarget){
	if(totalCountTarget_ != totalCountTarget)
		totalCountTarget_ = totalCountTarget;
}

void REIXSXESMCPDetector::onControlsConnected(bool connected){
	if(connected)
		setConnected(connected);
	else
		setConnected(false);

	if(connected)
		setReadyForAcquisition();
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void REIXSXESMCPDetector::onControlsTimedOut(){
	setConnected(false);
}

void REIXSXESMCPDetector::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode){
	acquire(readMode);
}

void REIXSXESMCPDetector::onDwellTimeSourceSetDwellTime(double dwellSeconds){
	if(!isConnected())
		return;

	if(dwellSeconds != acquisitionTime())
		setAcquisitionTime(dwellSeconds);
	else
		dwellTimeSource_->setSucceeded();
}

void REIXSXESMCPDetector::onDwellTimeTimerTimeout(){
	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalCounts))
		disconnect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalCountsControlValueChanged(double)));
	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalTime)){
		dwellTimeTimer_->deleteLater();
		dwellTimeTimer_ = 0;
	}
	setAcquisitionSucceeded();
}

void REIXSXESMCPDetector::onTotalCountsControlValueChanged(double totalCounts){
	if(totalCounts >= totalCountTarget_){
		if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalCounts))
			disconnect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalCountsControlValueChanged(double)));
		if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalTime)){
			dwellTimeTimer_->stop();
			dwellTimeTimer_->deleteLater();
			dwellTimeTimer_ = 0;
		}

		setAcquisitionSucceeded();

		if(!isConnected() && !isNotReadyForAcquisition())
			setNotReadyForAcquisition();
		else if(isConnected() && !isReadyForAcquisition())
			setReadyForAcquisition();
	}
}

bool REIXSXESMCPDetector::initializeImplementation(){
	setInitialized();
	return true;
}

bool REIXSXESMCPDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	if(clearOnStart_)
		clear();

	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalTime)){
		dwellTimeTimer_ = new QTimer();
		dwellTimeTimer_->setSingleShot(true);
		dwellTimeTimer_->setInterval(dwellTime_*1000);
	}
	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalCounts))
		connect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalCountsControlValueChanged(double)));

	setAcquiring();
	return true;
}

bool REIXSXESMCPDetector::cleanupImplementation(){
	setCleanedUp();
	return true;
}

bool REIXSXESMCPDetector::clearImplementation(){
	if(!isConnected())
		return false;
	if(clearControl_->move(1) == AMControl::NoFailure)
		return true;
	return false;
}

void REIXSXESMCPDetector::setFinishedCondition(REIXSXESMCPDetector::XESMCPFinishedConditions finishedConditions){
	if(finishedConditions_ != finishedConditions)
		finishedConditions_ = finishedConditions;
}
