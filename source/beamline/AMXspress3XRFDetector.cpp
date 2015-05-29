#include "AMXspress3XRFDetector.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

AMXspress3XRFDetector::AMXspress3XRFDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
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
}

AMXspress3XRFDetector::~AMXspress3XRFDetector()
{

}

bool AMXspress3XRFDetector::initializeImplementation()
{
	setInitializing();

	AMListAction3 *initializeAction = new AMListAction3(new AMListActionInfo3("Arm detector", "Arm detector"));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 1.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(updateControl_, 1.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 0.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(updateControl_, 0.0));
	initializeAction->addSubAction(AMActionSupport::buildControlMoveAction(initializationControl_, 1.0));

	connect(initializeAction, SIGNAL(failed()), initializeAction, SLOT(scheduleForDeletion()));
	connect(initializeAction, SIGNAL(cancelled()), initializeAction, SLOT(scheduleForDeletion()));
	connect(initializeAction, SIGNAL(succeeded()), initializeAction, SLOT(scheduleForDeletion()));

	connect(initializeAction, SIGNAL(failed()), this, SLOT(setInitializationRequired()));
	connect(initializeAction, SIGNAL(cancelled()), this, SLOT(setInitializationRequired()));
	connect(initializeAction, SIGNAL(succeeded()), this, SLOT(setInitialized()));

	initializeAction->start();

	return true;
}

QString AMXspress3XRFDetector::synchronizedDwellKey() const
{
	return "";
}

bool AMXspress3XRFDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool AMXspress3XRFDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
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

	else if (acquisitionStatusControl_->withinTolerance(0) || acquisitionStatusControl_->withinTolerance(5) || acquisitionStatusControl_->withinTolerance(10))
		setNotReadyForAcquisition();

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
	setInitializationRequired();
	setNotReadyForAcquisition();
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

	foreach (AMControl *control, spectraControls_)
		connect(control, SIGNAL(valueChanged(double)), this, SLOT(onDataChanged()));
}

void AMXspress3XRFDetector::onDataChanged()
{
	if (!dataReady_ && isAcquiring()){

		dataReadyCounter_--;

		if (dataReadyCounter_ == 0){

			dataReady_ = true;
			setAcquisitionSucceeded();

			if (acquisitionStatusControl_->withinTolerance(0) && acquireControl_->withinTolerance(0))
				setInitializationRequired();

			setReadyForAcquisition();
		}
	}
}
