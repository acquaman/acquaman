/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSXESMCPDetector.h"

#include <QTimer>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "util/AMErrorMonitor.h"
#include "beamline/AMDetectorTriggerSource.h"

 REIXSXESMCPDetector::~REIXSXESMCPDetector(){}
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

	vetoControl_ = new AMSinglePVControl("Veto", "PDTR1610-4-I21-01:OprVeto", this, 0.5);
	vetoStateControl_ = new AMReadOnlyPVControl("VetoState", "PDTR1610-4-I21-01:Veto:state", this);

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
	allControls_->addControl(vetoControl_);
	allControls_->addControl(vetoStateControl_);

	triggerSource_ = new AMDetectorTriggerSource(QString("REIXSXESMCPTriggerSource"), this);
	dwellTimeSource_ = new AMDetectorDwellTimeSource(QString("REIXSXESMCPDwellTimeSource"), this);

	connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellTimeSourceSetDwellTime(double)));

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(onControlsTimedOut()));

	connect(imageControl_->readPV(), SIGNAL(valueChanged()), this, SIGNAL(imageDataChanged()));
	connect(instantaneousImageControl_->readPV(), SIGNAL(valueChanged()), this, SIGNAL(instantaneousImageDataChanged()));
	connect(countsPerSecondControl_, SIGNAL(valueChanged(double)), this, SIGNAL(countsPerSecondChanged(double)));
	connect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SIGNAL(totalCountsChanged(double)));
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

bool REIXSXESMCPDetector::data(double *outputValues) const
{
	QVector<int> intVal = imageControl_->readPV()->lastIntegerValues();

	for(int x = 0, size = intVal.size(); x < size; x++)
		outputValues[x] = double(intVal.at(x));

	return true;
}

QVector<int> REIXSXESMCPDetector::imageData() const{
	return imageControl_->readPV()->lastIntegerValues();
}

QVector<int> REIXSXESMCPDetector::instantaneousImageData() const{
	return instantaneousImageControl_->readPV()->lastIntegerValues();
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

bool REIXSXESMCPDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	if(readMode != AMDetectorDefinitions::SingleRead)
		return false;

	return true;
}

bool REIXSXESMCPDetector::setAcquisitionTime(double seconds){
	if(seconds < 0.1)
		return false;
	dwellTime_ = seconds;
	return true;
}

bool REIXSXESMCPDetector::addAcquisitionTime(double addSeconds){
	if(addSeconds < 0.1)
		return false;
	if (!dwellTimeTimer_->isActive())
		return false;

	dwellTime_+= addSeconds;

	dwellTimeTimer_->blockSignals(true);
	dwellTimeTimer_->setInterval(dwellTime_*1000);
	dwellTimeTimer_->blockSignals(false);

	return true;
}


AMControl* REIXSXESMCPDetector::averagingPeriodControl(){
	return averagingPeriodSecsControl_;
}

AMControl* REIXSXESMCPDetector::persistDurationControl(){
	return persistTimeSecsControl_;
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

void REIXSXESMCPDetector::pauseDwelling()
{
	if (dwellTimeTimer_) {
		dwellTimeTimer_->pause();
	}

	toggleVeto(false);
}

void REIXSXESMCPDetector::resumeDwelling()
{
	if (dwellTimeTimer_) {
		dwellTimeTimer_->resume();
	}

	toggleVeto(on);
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

double REIXSXESMCPDetector::acquisitionTimeTolerance() const
{
	return 0.001;	// This can probably be fine tuned to something more real.
}

void REIXSXESMCPDetector::onDwellTimeSourceSetDwellTime(double dwellSeconds){
	if(!isConnected())
		return;

	if(!acquisitionTimeWithinTolerance(dwellSeconds))
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
	acquisitionSucceededHelper();
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

		acquisitionSucceededHelper();
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

	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalCounts)){
		connect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalCountsControlValueChanged(double)));
	}
	if(finishedConditions_.testFlag(REIXSXESMCPDetector::FinishedTotalTime)){
		dwellTimeTimer_ = new AMTimer();
		dwellTimeTimer_->setSingleShot(true);
		dwellTimeTimer_->setInterval(int(dwellTime_*1000));
		connect(dwellTimeTimer_, SIGNAL(timeout()), this, SLOT(onDwellTimeTimerTimeout()));
		dwellTimeTimer_->start();
	}

	setAcquiring();
	return true;
}

bool REIXSXESMCPDetector::cancelAcquisitionImplementation(){
	if(!isConnected())
		return false;
	dwellTimeTimer_->stop();
	disconnect(totalCountsControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalCountsControlValueChanged(double)));
	dwellTimeTimer_->deleteLater();
	dwellTimeTimer_ = 0;
	acquisitionCancelledHelper();
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

void REIXSXESMCPDetector::acquisitionSucceededHelper(){
	setAcquisitionSucceeded();

	if(!isConnected() && !isNotReadyForAcquisition())
		setNotReadyForAcquisition();
	else if(isConnected() && !isReadyForAcquisition())
		setReadyForAcquisition();

	emit imageDataChanged();
}

void REIXSXESMCPDetector::acquisitionCancelledHelper(){
	setAcquisitionCancelled();

	if(!isConnected() && !isNotReadyForAcquisition())
		setNotReadyForAcquisition();
	else if(isConnected() && !isReadyForAcquisition())
		setReadyForAcquisition();

	emit imageDataChanged();
}

void REIXSXESMCPDetector::toggleVeto(bool on)
{
	if (vetoControl_->isConnected() && vetoStateControl_->isConnected() && vetoStateControl_->value() == 1) {
		if (on)
			vetoControl_->move(0);
		else
			vetoControl_->move(1);
	}
}
