#include "AMDetector.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMDetector::AMDetector(const QString &name, const QString &description, QObject *parent) :
	QObject(parent)
{
	setObjectName(name);
	description_ = description;

	connected_ = false;
	powered_ = false;

	acquisitionState_ = AMDetector::NotReadyForAcquisition;
	initializationState_ = AMDetector::InitializationRequired;
	cleanupState_ = AMDetector::CleanupRequired;

	autoSetInitializing_ = true;
	autoSetCancelling_ = true;
	autoSetCleaningUp_ = true;
}

AMDetectorInfo AMDetector::toInfo() const{
	return AMDetectorInfo(name(), description(), units(), acquisitionTime(), readMode());
}

AMDetector::operator AMMeasurementInfo() {
	// This is code included from the previous AMDetector(Info) system. This may be important for the transition. [DKC January 14th, 2013]
	//return AMMeasurementInfo(name(), description(), units(), axes());
	if(!description().isEmpty())
		return AMMeasurementInfo(description().remove(" "), description(), units(), axes());
	else
		return AMMeasurementInfo(name(), name(), units(), axes());
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

bool AMDetector::currentlySynchronizedDwell() const{
	if(AMBeamline::bl()->synchronizedDwellTime()){
		int index = AMBeamline::bl()->synchronizedDwellTime()->indexOfDetector(this);
		if(index >= 0)
			return true;
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

	return true;
}

bool AMDetector::reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	return true;
}

bool AMDetector::readingND(int dimensionality, const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	switch(dimensionality) {
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

AMAction3* AMDetector::createInitializationActions(){
	return new AMDetectorInitializeAction(new AMDetectorInitializeActionInfo(toInfo()), this);
}

AMAction3* AMDetector::createAcquisitionAction(AMDetectorDefinitions::ReadMode readMode){
	return new AMDetectorAcquisitionAction(new AMDetectorAcquisitionActionInfo(toInfo(), readMode, this));
}

AMAction3* AMDetector::createCleanupActions(){
	return new AMDetectorCleanupAction(new AMDetectorCleanupActionInfo(toInfo()), this);
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

	if(startIndex.rank() != detectorRank || endIndex.rank() != detectorRank || startIndex.rank() != endIndex.rank())
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

/////////////////////////
// INITIALIZE ACTION ///
///////////////////////

AMDetectorInitializeActionInfo::AMDetectorInitializeActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3("Detector Initialization", "Detector Initialization", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	QString description = QString("Initialize %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorInitializeActionInfo::AMDetectorInitializeActionInfo(const AMDetectorInitializeActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}

AMDetectorInitializeAction::AMDetectorInitializeAction(AMDetectorInitializeActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else{
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
}

AMDetectorInitializeAction::AMDetectorInitializeAction(const AMDetectorInitializeAction &other) :
	AMAction3(other)
{
	const AMDetectorInitializeActionInfo *info = qobject_cast<const AMDetectorInitializeActionInfo*>(other.info());

	if(info){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
	else
		detector_ = 0;
}

void AMDetectorInitializeAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORINITIALIZEACTION_NO_VALID_DETECTOR,
						  QString("There was an error initializing the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorInitializeInfo()->name()));
		setFailed();
		return;
	}

	// connect to detector initialization signals
	connect(detector_, SIGNAL(initializeStarted()), this, SLOT(onInitializeStarted()));
	connect(detector_, SIGNAL(initializeFinished(bool)), this, SLOT(onInitializeFinished(bool)));

	detector_->initialize();
}

void AMDetectorInitializeAction::onInitializeStarted(){
	disconnect(detector_, SIGNAL(initializeStarted()), this, SLOT(onInitializeStarted()));

	setStarted();
}

void AMDetectorInitializeAction::onInitializeFinished(bool initializationSucceeded){
	disconnect(detector_, 0, this, 0);

	if(initializationSucceeded)
		setSucceeded();
	else
		setFailed();
}

void AMDetectorInitializeAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

///////////////////////////////
// (END) INITIALIZE ACTION ///
/////////////////////////////

///////////////////////////
// ACQUISITION ACTION ///
///////////////////////

AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorInfo &detectorInfo, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMActionInfo3("Detector Acquisition", "Detector Acquisition", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);
	readMode_ = readMode;

	QString description = QString("Acquire %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorAcquisitionActionInfo::AMDetectorAcquisitionActionInfo(const AMDetectorAcquisitionActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
	readMode_ = other.readMode();
}

AMDetectorAcquisitionAction::AMDetectorAcquisitionAction(AMDetectorAcquisitionActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else{
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
}

AMDetectorAcquisitionAction::AMDetectorAcquisitionAction(const AMDetectorAcquisitionAction &other) :
	AMAction3(other)
{
	const AMDetectorAcquisitionActionInfo *info = qobject_cast<const AMDetectorAcquisitionActionInfo*>(other.info());

	if(info){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
	else
		detector_ = 0;
}

void AMDetectorAcquisitionAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_NO_VALID_DETECTOR,
						  QString("There was an error acquiring with the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}

	if( (detectorAcquisitionInfo()->readMode() != AMDetectorDefinitions::SingleRead) && !detector_->canContinuousAcquire()){
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_NOT_VALID_READMODE,
						  QString("There was an error acquiring with the detector '%1', because continuous reads are not supported for this detector. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}

	// connect to detector initialization signals
	connect(detector_, SIGNAL(acquisitionStarted(bool)), this, SLOT(onAcquisitionStarted(bool)));
	connect(detector_, SIGNAL(acquisitionFinished(bool)), this, SLOT(onAcquisitionFinished(bool)));

	detector_->acquire(detectorAcquisitionInfo()->readMode());
}

void AMDetectorAcquisitionAction::onAcquisitionStarted(bool acquisitionStartSuccessful){
	disconnect(detector_, SIGNAL(acquisitionStarted(bool)), this, SLOT(onAcquisitionStarted(bool)));

	if(acquisitionStartSuccessful)
		setStarted();
	else{
		AMErrorMon::alert(this,
						  AMDETECTORACQUISITIONACTION_ACQUISITION_START_FAILED,
						  QString("There was an error acquiring with the detector '%1', because the acquisition failed to start. Please report this problem to the Acquaman developers.").arg(detectorAcquisitionInfo()->name()));
		setFailed();
		return;
	}
}

void AMDetectorAcquisitionAction::onAcquisitionFinished(bool acquisitionSucceeded){
	disconnect(detector_, 0, this, 0);

	if(acquisitionSucceeded)
		setSucceeded();
	else
		setFailed();
}

void AMDetectorAcquisitionAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

/////////////////////////////////
// (END) ACQUISITION ACTION ///
//////////////////////////////

//////////////////////
// CLEANUP ACTION ///
////////////////////

AMDetectorCleanupActionInfo::AMDetectorCleanupActionInfo(const AMDetectorInfo &detectorInfo, QObject *parent) :
	AMActionInfo3("Detector Cleanup", "Detector Cleanup", ":/system-run.png", parent)
{
	detectorInfo_.setValuesFrom(detectorInfo);

	QString description = QString("Cleanup %1").arg(detectorInfo_.description());
	setShortDescription(description);
	setLongDescription(description);
}

AMDetectorCleanupActionInfo::AMDetectorCleanupActionInfo(const AMDetectorCleanupActionInfo &other) :
	AMActionInfo3(other)
{
	detectorInfo_.setValuesFrom(*(other.detectorInfo()));
}

AMDetectorCleanupAction::AMDetectorCleanupAction(AMDetectorCleanupActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else{
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
}

AMDetectorCleanupAction::AMDetectorCleanupAction(const AMDetectorCleanupAction &other) :
	AMAction3(other)
{
	const AMDetectorCleanupActionInfo *info = qobject_cast<const AMDetectorCleanupActionInfo*>(other.info());

	if(info){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}
	else
		detector_ = 0;
}

void AMDetectorCleanupAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_){
		// GRAB IT FROM BEAMLINE INTERFACE
		// detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	}

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORCLEANUPACTION_NO_VALID_DETECTOR,
						  QString("There was an error cleaning up the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorCleanupInfo()->name()));
		setFailed();
		return;
	}

	// connect to detector initialization signals
	connect(detector_, SIGNAL(cleanupStarted()), this, SLOT(onCleanupStarted()));
	connect(detector_, SIGNAL(cleanupFinished(bool)), this, SLOT(onCleanupFinished(bool)));

	detector_->cleanup();
}

void AMDetectorCleanupAction::onCleanupStarted(){
	disconnect(detector_, SIGNAL(cleanupStarted()), this, SLOT(onCleanupStarted()));

	setStarted();
}

void AMDetectorCleanupAction::onCleanupFinished(bool cleanupSucceeded){
	disconnect(detector_, 0, this, 0);

	if(cleanupSucceeded)
		setSucceeded();
	else
		setFailed();
}

void AMDetectorCleanupAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

///////////////////////////////
// (END) INITIALIZE ACTION ///
/////////////////////////////
