#include "AMDetector.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"
#include "actions3/actions/AMDetectorSetAsDarkCurrentCorrectionAction.h"
#include "actions3/actions/AMDetectorInitializeAction.h"
#include "actions3/actions/AMDetectorDwellTimeAction.h"
#include "actions3/actions/AMDetectorAcquisitionAction.h"
#include "actions3/actions/AMDetectorTriggerAction.h"
#include "actions3/actions/AMDetectorReadAction.h"
#include "actions3/actions/AMDetectorCleanupAction.h"


 AMDetector::~AMDetector(){}
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

	isVisible_ = true;
	hiddenFromUsers_ = false;

    darkCurrentMeasurementValue_ = 0;
    darkCurrentMeasurementTime_ = 0;
    requiresNewDarkCurrentMeasurement_ = true;

	QTimer::singleShot(0, this, SLOT(initiateTimedOutTimer()));
}

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

double AMDetector::darkCurrentMeasurementValue() const {
    if (canDoDarkCurrentCorrection())
        return darkCurrentMeasurementValue_;

    return -1;
}

int AMDetector::darkCurrentMeasurementTime() const {
    if (canDoDarkCurrentCorrection())
        return darkCurrentMeasurementTime_;

    return -1;
}

bool AMDetector::requiresNewDarkCurrentMeasurement() const {
    if (canDoDarkCurrentCorrection())
        return requiresNewDarkCurrentMeasurement_;

    return false;
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

//	qDebug() << "Has synchronized dwell time: " << AMBeamline::bl()->synchronizedDwellTime();
	if(AMBeamline::bl()->synchronizedDwellTime()){
		int index = AMBeamline::bl()->synchronizedDwellTime()->indexOfDetector(this);
		if(index >= 0)
			return AMBeamline::bl()->synchronizedDwellTime()->enabledAt(index);
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

bool AMDetector::lastContinuousReading(double *outputValues) const{
	if(!canContinuousAcquire())
		return false;
	return lastContinuousReadingImplementation(outputValues);
}

int AMDetector::lastContinuousSize() const{
	if(!canContinuousAcquire())
		return -1;
	return 0;
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

AMAction3* AMDetector::createDarkCurrentCorrectionActions(double dwellTime){
    Q_UNUSED(dwellTime)
    return 0;
}

AMAction3* AMDetector::createSetAsDarkCurrentCorrectionAction(){
    return new AMDetectorSetAsDarkCurrentCorrectionAction(new AMDetectorSetAsDarkCurrentCorrectionActionInfo(toInfo()), this);
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

void AMDetector::setAsDarkCurrentMeasurementValue(){
    if (canDoDarkCurrentCorrection()){
        darkCurrentMeasurementValue_ = double(singleReading()) / acquisitionTime();
        qDebug() << name() << " has new dark current correction measurement: " << darkCurrentMeasurementValue_;
        setRequiresNewDarkCurrentMeasurement(false);
        emit newDarkCurrentMeasurementValueReady(darkCurrentMeasurementValue_);
    }
}

void AMDetector::setAsDarkCurrentMeasurementTime(double lastTime) {
    if (canDoDarkCurrentCorrection()) {

        if (lastTime > darkCurrentMeasurementTime_)
            setRequiresNewDarkCurrentMeasurement(true);

        darkCurrentMeasurementTime_ = lastTime;

        qDebug() << name() << "has new last dark current correction time: " << lastTime;
    }
}

void AMDetector::setRequiresNewDarkCurrentMeasurement(bool needsNewDCC) {
    if (canDoDarkCurrentCorrection()) {
        if (needsNewDCC)
            qDebug() << "AMDetector::setRequiresNewDarkCurrentCorrection : " << name() << "requires new dark current correction measurement.";

        emit requiresNewDarkCurrentMeasurement(requiresNewDarkCurrentMeasurement_ = needsNewDCC);
    }
}

bool AMDetector::cancelAcquisitionImplementation(){
	return false;
}

bool AMDetector::lastContinuousReadingImplementation(double *outputValues) const{
	Q_UNUSED(outputValues)

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
	//if(!acceptableChangeAcquisitionState(newState))
	//	return;

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
	//if(!acceptableChangeInitializationState(newState))
	//	return;

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
	//if(!acceptableChangeCleanupState(newState))
	//	return;

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
