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


#include "AMDetector.h"

#include <math.h>

#include <QTimer>

//#include "beamline/AMBeamline.h"
#include "beamline/AMBeamlineSupport.h"
#include "beamline/AMSynchronizedDwellTime.h"

#include "util/AMErrorMonitor.h"
#include "actions3/actions/AMDetectorInitializeAction.h"
#include "actions3/actions/AMDetectorDwellTimeAction.h"
#include "actions3/actions/AMDetectorAcquisitionAction.h"
#include "actions3/actions/AMDetectorTriggerAction.h"
#include "actions3/actions/AMDetectorReadAction.h"
#include "actions3/actions/AMDetectorCleanupAction.h"
#include "actions3/actions/AMDetectorSetLastMeasurementAsDarkCurrentAction.h"
#include "actions3/actions/AMDetectorSetDarkCurrentValueAction.h"
#include "actions3/actions/AMDetectorSetDarkCurrentTimeAction.h"
#include "actions3/actions/AMDetectorSetDarkCurrentValidStateAction.h"

AMDetectorContinuousMotionRangeData::AMDetectorContinuousMotionRangeData(int motionStartIndex, int motionEndIndex, int listIndex)
{
	motionStartIndex_ = motionStartIndex;
	motionEndIndex_ = motionEndIndex;
	listIndex_ = listIndex;
}

bool AMDetectorContinuousMotionRangeData::isValid() const
{
	return ((motionStartIndex_ != -1) && (motionEndIndex_ != -1) && (listIndex_ != -1));
}

AMDetector::AMDetector(const QString &name, const QString &description, QObject *parent) :
	QObject(parent)
{
	setObjectName(name);
	description_ = description;

	connected_ = false;
	wasConnected_ = false;
	timedOut_ = false;
	timedOutTimer_ = 0; //NULL
	setTimeOutMS(AMDETECTOR_DEFAULT_TIMEOUT_MS);
	powered_ = false;

	acquisitionState_ = AMDetector::NotReadyForAcquisition;
	initializationState_ = AMDetector::InitializationRequired;
	cleanupState_ = AMDetector::CleanupRequired;

	autoSetInitializing_ = true;
	autoSetCancelling_ = true;
	autoSetCleaningUp_ = true;

	isVisible_ = false;
	hiddenFromUsers_ = false;

	darkCurrentValue_ = 0;
	darkCurrentTime_ = 0;
	darkCurrentValidState_ = true;

	QTimer::singleShot(0, this, SLOT(initiateTimedOutTimer()));
}

AMDetector::~AMDetector(){}

AMDetectorInfo AMDetector::toInfo() const{
	return AMDetectorInfo(name(), description(), units(), acquisitionTime(), readMode());
}

AMDetector::operator AMMeasurementInfo() {
	return AMMeasurementInfo(name(), description(), units(), axes());
	// This is code included from the previous AMDetector(Info) system. This may be important for the transition. [DKC January 14th, 2013]
	/*
	if(!description().isEmpty())
		return AMMeasurementInfo(description().remove(" "), description(), units(), axes());
	else
		return AMMeasurementInfo(name(), name(), units(), axes());
	*/
}

double AMDetector::darkCurrentValue() const {
	if (canDoDarkCurrentCorrection())
		return darkCurrentValue_;

	return -1;
}

double AMDetector::darkCurrentTime() const {
	if (canDoDarkCurrentCorrection())
		return darkCurrentTime_;

	return -1;
}

bool AMDetector::darkCurrentValidState() const {
	if (canDoDarkCurrentCorrection())
		return darkCurrentValidState_;

	return false;
}

bool AMDetector::darkCurrentValidState(double dwellTime) const
{
	bool result = false;

	if (canDoDarkCurrentCorrection() && darkCurrentTime_ < dwellTime)
		result = darkCurrentValidState_;

	return result;
}

QString AMDetector::acquisitionStateDescription(AMDetector::AcqusitionState state){
	switch(state){
	case NotReadyForAcquisition:
		return "Not Ready For Acquisition";
	case ReadyForAcquisition:
		return "Ready for Acquisition";
	case Acquiring:
		return "Acquiring";
	case Cancelling:
		return "Cancelling Acquisition";
	case Cancelled:
		return "Acquisition Cancelled";
	case Succeeded:
		return "Acquisition Succeeded";
	case Failed:
		return "Acquisition Failed";
	default:
		return "Invalid Acquisition State";
	}
}

QString AMDetector::initializationStateDescription(AMDetector::InitializationState state){
	switch(state){
	case InitializationRequired:
		return "Initialization Required";
	case Initializing:
		return "Initializing";
	case Initialized:
		return "Initialized";
	default:
		return "Invalid Initialization State";
	}
}

QString AMDetector::cleanupStateDescription(AMDetector::CleanupState state){
	switch(state){
	case CleanupRequired:
		return "Cleanup Required";
	case CleaningUp:
		return "Cleaning Up";
	case CleanedUp:
		return "Cleaned Up";
	default:
		return "Invalid Cleanup State";
	}
}

AMnDIndex AMDetector::size() const
{
	AMnDIndex index = AMnDIndex(axes_.size(), AMnDIndex::DoNotInit);

	for (int i = 0, size = index.rank(); i < size; i++)
		index[i] = axes_.at(i).size;

	return index;
}

int AMDetector::size(int axisNumber) const
{
	if (axisNumber < 0 || axisNumber >= axes_.size())
		return -1;

	return axes_.at(axisNumber).size;
}

bool AMDetector::currentlySynchronizedDwell() const{
//	if(AMBeamline::bl()->synchronizedDwellTime()){
//		int index = AMBeamline::bl()->synchronizedDwellTime()->indexOfDetector(this);
//		if(index >= 0)
//			return AMBeamline::bl()->synchronizedDwellTime()->enabledAt(index);
//	}
	if(AMBeamlineSupport::beamlineSynchronizedDwellTimeAPI() && AMBeamlineSupport::beamlineSynchronizedDwellTimeAPI()->synchronizedDwellTime()){
		int index = AMBeamlineSupport::beamlineSynchronizedDwellTimeAPI()->synchronizedDwellTime()->indexOfDetector(this);
		if(index >= 0)
			return AMBeamlineSupport::beamlineSynchronizedDwellTimeAPI()->synchronizedDwellTime()->enabledAt(index);
	}
	return false;
}

bool AMDetector::reading0D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	if(startIndex != endIndex)
		return false;

	AMNumber retVal = reading(startIndex);
	if(!retVal.isValid())
		return false;

	*outputValues = double(retVal);
	return true;
}

bool AMDetector::reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	if (endIndex.i() < startIndex.i())
		return false;

	for (int i = startIndex.i(); i <= endIndex.i(); i++){

		AMNumber retVal = reading(i);

		if(!retVal.isValid())
			return false;

		outputValues[i] = double(retVal);
	}

	return true;
}

bool AMDetector::reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	if (endIndex.i() < startIndex.i() || endIndex.j() < startIndex.j())
		return false;

	int iSize = size(0);

	for (int j = startIndex.j(); j <= endIndex.j(); j++){

		for (int i = startIndex.i(); i <= endIndex.i(); i++){

			AMNumber retVal = reading(AMnDIndex(i, j));

			if(!retVal.isValid())
				return false;

			outputValues[i+j*iSize] = double(retVal);
		}
	}

	return true;
}

bool AMDetector::readingND(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	switch(startIndex.rank()) {
	case 0:
		return reading0D(startIndex, endIndex, outputValues);
	case 1:
		return reading1D(startIndex, endIndex, outputValues);
	case 2:
		return reading2D(startIndex, endIndex, outputValues);
	default:
		return false;
	}
}

AMDSClientDataRequest* AMDetector::lastContinuousData(double seconds) const{
	Q_UNUSED(seconds)

	return 0;
}

bool AMDetector::setContinuousDataWindow(double continuousDataWindowSeconds){
	Q_UNUSED(continuousDataWindowSeconds)

	return false;
}

QString AMDetector::amdsBufferName() const
{
	return QString();
}

int AMDetector::amdsPollingBaseTimeMilliseconds() const
{
	return -1;
}

AMAction3* AMDetector::createInitializationActions(){
	return new AMDetectorInitializeAction(new AMDetectorInitializeActionInfo(toInfo()), this);
}

AMAction3* AMDetector::createSetAcquisitionTimeAction(double seconds){
	return new AMDetectorDwellTimeAction(new AMDetectorDwellTimeActionInfo(toInfo(), seconds), this);
}

AMAction3* AMDetector::createAcquisitionAction(AMDetectorDefinitions::ReadMode readMode){
	return new AMDetectorAcquisitionAction(new AMDetectorAcquisitionActionInfo(toInfo(), readMode, this));
}

AMAction3* AMDetector::createTriggerAction(AMDetectorDefinitions::ReadMode readMode){
	return new AMDetectorTriggerAction(new AMDetectorTriggerActionInfo(toInfo(), readMode, this));
}

AMAction3* AMDetector::createReadAction(){
	return new AMDetectorReadAction(new AMDetectorReadActionInfo(toInfo(), this));
}

AMAction3* AMDetector::createCleanupActions(){
	return new AMDetectorCleanupAction(new AMDetectorCleanupActionInfo(toInfo()), this);
}

AMAction3* AMDetector::createSetLastMeasurementAsDarkCurrentAction()
{
	return new AMDetectorSetLastMeasurementAsDarkCurrentAction(new AMDetectorSetLastMeasurementAsDarkCurrentActionInfo(toInfo()), this);
}

AMAction3* AMDetector::createSetDarkCurrentValueAction(double newValue)
{
	return new AMDetectorSetDarkCurrentValueAction(new AMDetectorSetDarkCurrentValueActionInfo(newValue, toInfo(), this), this);
}

AMAction3* AMDetector::createSetDarkCurrentTimeAction(double newTime)
{
	return new AMDetectorSetDarkCurrentTimeAction(new AMDetectorSetDarkCurrentTimeActionInfo(newTime, toInfo()), this);
}

AMAction3* AMDetector::createSetDarkCurrentValidStateAction(bool isValid)
{
	return new AMDetectorSetDarkCurrentValidStateAction(new AMDetectorSetDarkCurrentValidStateActionInfo(isValid, toInfo()), this);
}

void AMDetector::setInitializing(){
	if(!acceptableChangeInitializationState(Initializing))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_INITIALIZING_UNEXPECTEDLY, QString("An unexpected transition occured to Initializing. Detector name: %1. Current state: %2.").arg(name()).arg(initializationStateDescription(initializationState())) );
	setInitializationState(Initializing);
}

void AMDetector::setInitialized(){
	if(!acceptableChangeInitializationState(Initialized))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_INITIALIZED_UNEXPECTEDLY, QString("An unexpected transition occured to Initialized. Detector name: %1. Current state: %2.").arg(name()).arg(initializationStateDescription(initializationState())) );
	setInitializationState(Initialized);
}

void AMDetector::setInitializationRequired(){
	if(!acceptableChangeInitializationState(InitializationRequired))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_INITIALIZATIONREQUIRED_UNEXPECTEDLY, QString("An unexpected transition occured to Initialization Required. Detector name: %1. Current state: %2.").arg(name()).arg(initializationStateDescription(initializationState())) );
	setInitializationState(InitializationRequired);
}

void AMDetector::setAcquiring(){
	if(!acceptableChangeAcquisitionState(Acquiring))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_ACQUIRING_UNEXPECTEDLY, QString("An unexpected transition occured to Acquiring. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(Acquiring);
}

void AMDetector::setAcquisitionSucceeded(){
	if(!acceptableChangeAcquisitionState(Succeeded))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_SUCCEEDED_UNEXPECTEDLY, QString("An unexpected transition occured to Succeeded. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(Succeeded);
}

void AMDetector::setAcquisitionFailed(){
	if(!acceptableChangeAcquisitionState(Failed))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_FAILED_UNEXPECTEDLY, QString("An unexpected transition occured to Failed. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(Failed);
}

void AMDetector::setAcquisitionCancelling(){
	if(!acceptableChangeAcquisitionState(Cancelling))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_CANCELLING_UNEXPECTEDLY, QString("An unexpected transition occured to Cancelling. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(Cancelling);
}

void AMDetector::setAcquisitionCancelled(){
	if(!acceptableChangeAcquisitionState(Cancelled))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_CANCELLED_UNEXPECTEDLY, QString("An unexpected transition occured to Cancelled. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(Cancelled);
}

void AMDetector::setReadyForAcquisition(){
	if(!acceptableChangeAcquisitionState(ReadyForAcquisition))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_READYFORACQUISITION_UNEXPECTEDLY, QString("An unexpected transition occured to Ready For Acquisition. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(ReadyForAcquisition);
}

void AMDetector::setNotReadyForAcquisition(){
	if(!acceptableChangeAcquisitionState(NotReadyForAcquisition))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_NOTREADYFORACQUISITION_UNEXPECTEDLY, QString("An unexpected transition occured to Not Ready For Acquisition. Detector name: %1. Current state: %2.").arg(name()).arg(acquisitionStateDescription(acquisitionState())) );
	setAcquisitionState(NotReadyForAcquisition);
}

void AMDetector::setCleaningUp(){
	if(!acceptableChangeCleanupState(CleaningUp))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_CLEANINGUP_UNEXPECTEDLY, QString("An unexpected transition occured to Cleaning Up. Detector name: %1. Current state: %2.").arg(name()).arg(cleanupStateDescription(cleanupState())) );
	setCleanupState(CleaningUp);
}

void AMDetector::setCleanedUp(){
	if(!acceptableChangeCleanupState(CleanedUp))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_CLEANEDUP_UNEXPECTEDLY, QString("An unexpected transition occured to Cleaned Up. Detector name: %1. Current state: %2.").arg(name()).arg(cleanupStateDescription(cleanupState())) );
	setCleanupState(CleanedUp);
}

void AMDetector::setCleanupRequired(){
	if(!acceptableChangeCleanupState(CleanupRequired))
		AMErrorMon::debug(this, AMDETECTOR_NOTIFIED_CLEANUPREQUIRED_UNEXPECTEDLY, QString("An unexpected transition occured to Cleanup Required. Detector name: %1. Current state: %2.").arg(name()).arg(cleanupStateDescription(cleanupState())) );
	setCleanupState(CleanupRequired);
}

void AMDetector::updateDarkCurrentValue(double newValue)
{
	darkCurrentValue_ = newValue;
	emit darkCurrentValueChanged(darkCurrentValue_);
}

void AMDetector::updateDarkCurrentTime(double newTime)
{
	darkCurrentTime_ = newTime;
	emit darkCurrentTimeChanged(darkCurrentTime_);
}

void AMDetector::updateDarkCurrentValidState(bool newState)
{
	darkCurrentValidState_ = newState;
	emit darkCurrentValidStateChanged(darkCurrentValidState_);
}

bool AMDetector::initialize(){
	if(acceptableChangeInitializationState(Initializing)){
		if(autoSetInitializing_)
			setInitializationState(Initializing);
		return initializeImplementation();
	}
	return false;
}

bool AMDetector::acquire(AMDetectorDefinitions::ReadMode readMode){
	if( (readMode == AMDetectorDefinitions::ContinuousRead) && !canContinuousAcquire())
		return false;

	if(acceptableChangeAcquisitionState(Acquiring))
		return acquireImplementation(readMode);
	return false;
}

bool AMDetector::cancelAcquisition(){
	if(!canCancel())
		return false;

	if(acceptableChangeAcquisitionState(Cancelling)){
		if(autoSetCancelling_)
			setAcquisitionState(Cancelling);
		return cancelAcquisitionImplementation();
	}
	return false;
}

bool AMDetector::cleanup(){
	if(acceptableChangeCleanupState(CleaningUp)){
		if(autoSetCleaningUp_)
			setCleanupState(CleaningUp);
		return cleanupImplementation();
	}
	return false;
}

bool AMDetector::clear(){
	if(!canClear())
		return false;
	return clearImplementation();
}

void AMDetector::setLastMeasurementAsDarkCurrent()
{
	if (canDoDarkCurrentCorrection()) {
		double newTime = acquisitionTime();
		double newValue = double(singleReading()) / newTime;

		updateDarkCurrentTime(newTime);
		updateDarkCurrentValue(newValue);
		updateDarkCurrentValidState(true);
	}
}

void AMDetector::setDarkCurrentValue(double newValue) {
	if (canDoDarkCurrentCorrection()){
		updateDarkCurrentValue(newValue);
	}
}

void AMDetector::setDarkCurrentTime(double newTime) {
	if (canDoDarkCurrentCorrection()) {
		updateDarkCurrentTime(newTime);
	}
}

void AMDetector::setDarkCurrentValidState(bool isValid)
{
	if (canDoDarkCurrentCorrection()) {
		updateDarkCurrentValidState(isValid);
	}
}

void AMDetector::arm()
{
	emit armed();
}

bool AMDetector::cancelAcquisitionImplementation(){
	return false;
}

bool AMDetector::clearImplementation(){
	return false;
}

void AMDetector::setConnected(bool isConnected){
	if(isConnected && !wasConnected_){
		wasConnected_ = true;
		timedOutTimerCleanup();
	}
	if(isConnected != connected_){
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMDetector::setPowered(bool isPowered){
	if(isPowered != powered_){
		powered_ = isPowered;
		emit powered(powered_);
	}
}

bool AMDetector::checkValid(const AMnDIndex &startIndex, const AMnDIndex &endIndex) const{
	int detectorRank = rank();

	if(startIndex.rank() != detectorRank || startIndex.rank() != endIndex.rank())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for(int mu=0; mu<detectorRank; ++mu) {
		if(endIndex.at(mu) >= size(mu))
			return false;
		if(endIndex.at(mu) < startIndex.at(mu))
			return false;
	}
#endif
	return true;
}

void AMDetector::setTimeOutMS(int timeOutMS){
	timeOutMS_ = timeOutMS;
}

void AMDetector::setAcquisitionState(AcqusitionState newState){
	acquisitionState_ = newState;
	emit acquisitionStateChanged(acquisitionState_);

	switch(acquisitionState_){
	case NotReadyForAcquisition:
		emit notReadyForAcquisition();
		break;
	case ReadyForAcquisition:
		emit readyForAcquisition();
		break;
	case Acquiring:
		emit acquiring();
		break;
	case Cancelling:
		emit acquisitionCancelling();
		break;
	case Cancelled:
		emit acquisitionCancelled();
		break;
	case Succeeded:
		emit acquisitionSucceeded();
		break;
	case Failed:
		emit acquisitionFailed();
		break;
	default:
		break;
	}
}

bool AMDetector::acceptableChangeAcquisitionState(AcqusitionState newState) const{
	bool canTransition = false;

	switch(newState){
	case NotReadyForAcquisition:
		if(acquisitionState_ == ReadyForAcquisition || acquisitionState_ == Cancelled || acquisitionState_ == Succeeded || acquisitionState_ == Failed )
			canTransition = true;
		break;
	case ReadyForAcquisition:
		if(acquisitionState_ == NotReadyForAcquisition || acquisitionState_ == Cancelled || acquisitionState_ == Succeeded || acquisitionState_ == Failed )
			canTransition = true;
		break;
	case Acquiring:
		if(acquisitionState_ == ReadyForAcquisition)
			canTransition = true;
		break;
	case Cancelling:
		if(acquisitionState_ == Acquiring)
			canTransition = true;
		break;
	case Cancelled:
		if(acquisitionState_ ==  Cancelling)
			canTransition = true;
		break;
	case Succeeded:
		if(acquisitionState_ == Acquiring)
			canTransition = true;
		break;
	case Failed:
		if(acquisitionState_ == Acquiring)
			canTransition = true;
		break;
	default:
		break;
	}
	return canTransition;
}

void AMDetector::setInitializationState(InitializationState newState){
	initializationState_ = newState;
	emit initializationStateChanged(initializationState_);

	switch(initializationState_){
	case InitializationRequired:
		emit initializationRequired();
		break;
	case Initializing:
		emit initializing();
		break;
	case Initialized:
		emit initialized();
		break;
	default:
		break;
	}
}

bool AMDetector::acceptableChangeInitializationState(InitializationState newState) const{
	bool canTransition = false;
	switch(newState){
	case InitializationRequired:
		if(initializationState_ == Initializing || initializationState_ == Initialized)
			canTransition = true;
		break;
	case Initializing:
		if(initializationState_ == InitializationRequired)
			canTransition = true;
		break;
	case Initialized:
		if(initializationState_ == Initializing)
			canTransition = true;
		break;
	default:
		break;
	}
	return canTransition;
}

void AMDetector::setCleanupState(CleanupState newState){
	cleanupState_ = newState;
	emit cleanupStateChanged(cleanupState_);

	switch(cleanupState_){
	case CleanupRequired:
		emit cleanupRequired();
		break;
	case CleaningUp:
		emit cleaningUp();
		break;
	case CleanedUp:
		emit cleanedUp();
		break;
	default:
		break;
	}
}

bool AMDetector::acceptableChangeCleanupState(CleanupState newState) const{
	bool canTransition = false;
	switch(newState){
	case CleanupRequired:
		if(cleanupState_ == CleaningUp || cleanupState_ == CleanedUp)
			canTransition = true;
		break;
	case CleaningUp:
		if(cleanupState_ == CleanupRequired)
			canTransition = true;
		break;
	case CleanedUp:
		if(cleanupState_ == CleaningUp)
			canTransition = true;
		break;
	default:
		break;
	}
	return canTransition;
}

void AMDetector::timedOutTimerCleanup(){
	if(timedOutTimer_){
		timedOutTimer_->stop();
		disconnect(timedOutTimer_, SIGNAL(timeout()), this, SLOT(onTimedOutTimerTimedOut()));

		timedOutTimer_->deleteLater();
		timedOutTimer_ = 0; //NULL
	}
}

void AMDetector::initiateTimedOutTimer(){
	if(!isConnected()){
		timedOutTimer_ = new QTimer();
		connect(timedOutTimer_, SIGNAL(timeout()), this, SLOT(onTimedOutTimerTimedOut()));
		timedOutTimer_->start(timeOutMS_);
	}
}

void AMDetector::onTimedOutTimerTimedOut(){
	timedOutTimerCleanup();
	if(!wasConnected_){
		timedOut_ = true;
		emit timedOut();
		emit connected(false);
	}
}

void AMDetector::setIsVisible(bool visible)
{
	if (isVisible_ != visible){

		isVisible_ = visible;
		emit isVisibleChanged(isVisible_);
	}
}

void AMDetector::setHiddenFromUsers(bool hidden)
{
	if (hiddenFromUsers_ != hidden){

		hiddenFromUsers_ = hidden;
		emit isVisibleChanged(hiddenFromUsers_);
	}
}

AMDetectorContinuousMotionRangeData AMDetector::retrieveContinuousMotionRangeData(const QList<QVector<qint32> > &baseData, int expectedDuration, int threshold)
{
	Q_UNUSED(baseData)
	Q_UNUSED(expectedDuration)
	Q_UNUSED(threshold)
	AMDetectorContinuousMotionRangeData retVal = AMDetectorContinuousMotionRangeData();
	return retVal;
}

bool AMDetector::acquisitionTimeWithinTolerance(double value) const
{
	return fabs(value - acquisitionTime()) < acquisitionTimeTolerance();
}
