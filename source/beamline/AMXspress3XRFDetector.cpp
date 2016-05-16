#include "AMXspress3XRFDetector.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

AMXspress3XRFDetector::AMXspress3XRFDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	triggerSource_ = 0;

	autoInitialize_ = false;
	dataReady_ = false;
	dataReadyCounter_ = -1;

	units_ = "Counts";

	AMAxisInfo ai("Energy", 4096, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	elapsedTime_.start();
	elapsedTimeTimer_.setInterval(50);
	connect(&elapsedTimeTimer_, SIGNAL(timeout()), this, SLOT(onElapsedTimerTimeout()));
	connect(this, SIGNAL(acquiring()), this, SLOT(startElapsedTime()));
	connect(this, SIGNAL(acquisitionCancelled()), this, SLOT(stopElapsedTime()));
	connect(this, SIGNAL(acquisitionFailed()), this, SLOT(stopElapsedTime()));
	connect(this, SIGNAL(acquisitionSucceeded()), this, SLOT(stopElapsedTime()));

	statusMessage_ = "";

	isTriggered_ = false;
}

AMXspress3XRFDetector::~AMXspress3XRFDetector()
{

}

QString AMXspress3XRFDetector::details() const
{
	return QString("%1\nAcquisition Time: %2 seconds\nFrame %3 of %4\n\n")
			.arg(description())
			.arg(acquisitionTime())
			.arg(currentFrame()+1)
			.arg(framesPerAcquisition());
}

bool AMXspress3XRFDetector::initializeImplementation()
{
	AMAction3 *initializeAction = createInitializationAction();

	connect(initializeAction, SIGNAL(failed()), initializeAction, SLOT(scheduleForDeletion()));
	connect(initializeAction, SIGNAL(cancelled()), initializeAction, SLOT(scheduleForDeletion()));
	connect(initializeAction, SIGNAL(succeeded()), initializeAction, SLOT(scheduleForDeletion()));

	initializeAction->start();

	return true;
}

QString AMXspress3XRFDetector::synchronizedDwellKey() const
{
	return "";
}

bool AMXspress3XRFDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void AMXspress3XRFDetector::setTriggerSource(AMZebraDetectorTriggerSource *triggerSource)
{
	if(triggerSource_)
		disconnect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));

	triggerSource_ = 0;

	if(triggerSource){
		triggerSource_ = triggerSource;
		connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	}
}

void AMXspress3XRFDetector::updateAcquisitionState()
{
	if (!isAcquiring() && initializationControl_->withinTolerance(0)){

		setInitializationRequired();
		setNotReadyForAcquisition();
	}

	else if (!isAcquiring() && acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(1)){

		dataReady_ = false;
		dataReadyCounter_ = elements();
		setAcquiring();
	}

	else if (isInitialized() && isNotReadyForAcquisition() && (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(0)))
		setReadyForAcquisition();

	else if (isNotReadyForAcquisition() && requiresInitialization() && autoInitialize_)
		initialize();
}

double AMXspress3XRFDetector::elapsedTime() const
{
	return elapsedTime_.elapsed();
}

void AMXspress3XRFDetector::startElapsedTime()
{
	elapsedTime_.restart();
	elapsedTimeTimer_.start();
	emit elapsedTimeChanged(0.0);
}

void AMXspress3XRFDetector::stopElapsedTime()
{
	elapsedTimeTimer_.stop();
	emit elapsedTimeChanged(double(elapsedTime_.elapsed())/1000.0);
}

void AMXspress3XRFDetector::onElapsedTimerTimeout()
{
	emit elapsedTimeChanged(double(elapsedTime_.elapsed())/1000.0);
}

void AMXspress3XRFDetector::onStatusMessageChanged()
{
	QString name;
	const AMProcessVariable *pv = statusMessageControl_->readPV();

	for (unsigned i = 0; i < pv->count(); i++){

		int current = pv->getInt(i);

		if (current == 0)
			break;

		name += QString::fromAscii((const char *) &current);
	}

	statusMessage_ = name;
	emit statusMessageChanged(statusMessage_);
}

bool AMXspress3XRFDetector::cancelAcquisitionImplementation()
{
	acquireControl_->move(0);
	setAcquisitionCancelled();
	return true;
}

void AMXspress3XRFDetector::setFramesPerAcquisition(int count)
{
	if (framesPerAcquisition() != count)
		framesPerAcquisitionControl_->move(count);
}

void AMXspress3XRFDetector::setThreshold(int newThreshold)
{
	foreach (AMControl *control, thresholdControls_)
		control->move(newThreshold);
}

void AMXspress3XRFDetector::disarm()
{
	initializationControl_->move(0);
}

void AMXspress3XRFDetector::makeConnections()
{
	allControlsCreated();

	connect(thresholdControls_.at(0), SIGNAL(valueChanged(double)), this, SIGNAL(thresholdChanged()));

	disconnect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(acquireControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));

	connect(statusMessageControl_, SIGNAL(connected(bool)), this, SLOT(onStatusMessageChanged()));
	connect(statusMessageControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusMessageChanged()));
	connect(currentFrameCountControl_, SIGNAL(valueChanged(double)), this, SIGNAL(currentFrameCountChanged()));
	connect(framesPerAcquisitionControl_, SIGNAL(valueChanged(double)), this, SIGNAL(framesPerAcquisitionChanged()));

	allControls_->addControl(initializationControl_);
	allControls_->addControl(statusMessageControl_);
	allControls_->addControl(currentFrameCountControl_);
	allControls_->addControl(framesPerAcquisitionControl_);

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(10);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(updateAcquisitionState()));

	connect(dataSource()->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataChanged()));
}

AMAction3* AMXspress3XRFDetector::createEraseAction()
{
	// Build the erase action, manually toggling the erase control.

	AMListAction3* eraseAction = new AMListAction3(new AMListActionInfo3("Erase detector data", "Erase detector data"), AMListAction3::Sequential);
	eraseAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 1.0));
	eraseAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 0.0));

	return eraseAction;
}

void AMXspress3XRFDetector::onDataChanged()
{
	if (!dataReady_ && isAcquiring()){

		dataReadyCounter_--;

		if (dataReadyCounter_ == 0){

			dataReady_ = true;
			setAcquisitionSucceeded();

			if (isTriggered_){

				isTriggered_ = false;
				triggerSource_->setSucceeded(this);
			}

			if (acquisitionStatusControl_->withinTolerance(0) && acquireControl_->withinTolerance(0)){

				setInitializationRequired();
				setNotReadyForAcquisition();
			}

			else
				setReadyForAcquisition();
		}
	}
}

void AMXspress3XRFDetector::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)
	isTriggered_ = true;
}

AMAction3 * AMXspress3XRFDetector::createInitializationAction()
{
	AMListAction3 *initializeAction = new AMListAction3(new AMListActionInfo3("Arm detector", "Arm detector"));
	initializeAction->addSubAction(createEraseAction());
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(updateControl_, 1.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(updateControl_, 0.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(initializationControl_, 1.0));

	connect(initializeAction, SIGNAL(failed()), this, SLOT(setInitializationRequired()));
	connect(initializeAction, SIGNAL(cancelled()), this, SLOT(setInitializationRequired()));
	connect(initializeAction, SIGNAL(succeeded()), this, SLOT(setInitialized()));

	return initializeAction;
}

AMAction3 * AMXspress3XRFDetector::createFramesPerAcquisitionAction(int number)
{
	if (!framesPerAcquisitionControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(framesPerAcquisitionControl_, number);
}

AMAction3 * AMXspress3XRFDetector::createDisarmAction()
{
	if (!initializationControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(initializationControl_, 0);
}
