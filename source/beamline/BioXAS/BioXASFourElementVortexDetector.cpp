#include "BioXASFourElementVortexDetector.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASFourElementVortexDetector::BioXASFourElementVortexDetector(const QString &name, const QString &description, QObject *parent)
	: AMXRFDetector(name, description, parent)
{
	autoInitialize_ = false;

	units_ = "Counts";

	AMAxisInfo ai("Energy", 4096, "Energy", "eV");
	ai.start = AMNumber(0);
	ai.increment = 10;
	ai.isUniform = true;
	axes_ << ai;

	// Stuff required by AMXRFDetector.
	acquireControl_ = new AMSinglePVControl("Acquisition Time", "BL1607-5-I22:SetPort_1", this, 0.5);
	acquisitionStatusControl_ = new AMReadOnlyPVControl("Status", "DXP1607-801:DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Integration Time", "BL1607-5-I22:SetPort_1.HIGH", this, 0.001);
	elapsedTimeControl_ = 0;

	for (int i = 0; i < 4; i++){

		channelEnableControls_.append(new AMSinglePVControl(QString("Channel Enable %1").arg(i+1), QString("DXP1607-801:C%1_PluginControlVal").arg(i+1), this, 0.1));
//		icrControls_.append(new AMReadOnlyPVControl(QString("Input Counts %1").arg(i+1), QString("DXP1607-801:C%1_SCA3:Value_RBV").arg(i+1), this, QString("The input counts for element %1 of the four element.").arg(i+1)));
//		ocrControls_.append(new AMReadOnlyPVControl(QString("Output Counts %1").arg(i+1), QString("DXP1607-801:C%1_SCA4:Value_RBV").arg(i+1), this, QString("The output counts for element %1 of the four element.").arg(i+1)));
		spectraControls_.append(new AMReadOnlyPVControl(QString("Raw Spectrum %1").arg(i+1), QString("DXP1607-801:ARR%1:ArrayData").arg(i+1), this));

		thresholdControls_.append(new AMPVControl(QString("Threshold %1").arg(i+1), QString("DXP1607-801:C%1_SCA4_THRESHOLD_RBV").arg(i+1), QString("DXP1607-801:C%1_SCA4_THRESHOLD").arg(i+1), QString(), this, 0.5));
	}

	allControlsCreated();

	connect(thresholdControls_.at(0), SIGNAL(valueChanged(double)), this, SIGNAL(thresholdChanged()));

	initializationControl_ = new AMSinglePVControl("Initialization", "DXP1607-801:Acquire", this, 0.1);
//	connect(this, SIGNAL(initializationRequired()), this, SLOT(initialize()));

	eraseControl_ = new AMSinglePVControl("EraseControl", "DXP1607-801:ERASE", this, 0.5);
	updateControl_ = new AMSinglePVControl("UpdateControl", "DXP1607-801:UPDATE", this, 0.5);

	disconnect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(acquireControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(updateAcquisitionState()));

	elapsedTime_.start();
	elapsedTimeTimer_.setInterval(50);
	connect(&elapsedTimeTimer_, SIGNAL(timeout()), this, SLOT(onElapsedTimerTimeout()));
	connect(this, SIGNAL(acquiring()), this, SLOT(startElapsedTime()));
	connect(this, SIGNAL(acquisitionCancelled()), this, SLOT(stopElapsedTime()));
	connect(this, SIGNAL(acquisitionFailed()), this, SLOT(stopElapsedTime()));
	connect(this, SIGNAL(acquisitionSucceeded()), this, SLOT(stopElapsedTime()));

	statusMessage_ = "";
	statusMessageControl_ = new AMReadOnlyPVControl("StatusMessage", "DXP1607-801:StatusMessage_RBV", this);
	connect(statusMessageControl_, SIGNAL(connected(bool)), this, SLOT(onStatusMessageChanged()));
	connect(statusMessageControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusMessageChanged()));

	currentFrameCountControl_ = new AMReadOnlyPVControl("CurrentFrame", "DXP1607-801:ArrayCounter_RBV", this);
	connect(currentFrameCountControl_, SIGNAL(valueChanged(double)), this, SIGNAL(currentFrameCountChanged()));

	framesPerAcquisitionControl_ = new AMPVControl("FramesPerAcquisition", "DXP1607-801:NumImages_RBV", "DXP1607-801:NumImages", QString(), this, 0.5);
	connect(framesPerAcquisitionControl_, SIGNAL(valueChanged(double)), this, SIGNAL(framesPerAcquisitionChanged()));

	allControls_->addControl(initializationControl_);
	allControls_->addControl(statusMessageControl_);
	allControls_->addControl(currentFrameCountControl_);
	allControls_->addControl(framesPerAcquisitionControl_);

	foreach (AMDataSource *source, rawSpectraSources_)
		((AM1DProcessVariableDataSource *)source)->setScale(10);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(updateAcquisitionState()));
}

BioXASFourElementVortexDetector::~BioXASFourElementVortexDetector()
{

}

bool BioXASFourElementVortexDetector::initializeImplementation()
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

QString BioXASFourElementVortexDetector::synchronizedDwellKey() const
{
	return "";
}

bool BioXASFourElementVortexDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool BioXASFourElementVortexDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

void BioXASFourElementVortexDetector::updateAcquisitionState()
{
	if (!isAcquiring() && initializationControl_->withinTolerance(0)){

		setInitializationRequired();
		setNotReadyForAcquisition();
	}

	else if (isInitialized() && (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(0))){

//		setInitialized();
		setReadyForAcquisition();
	}

	else if (isAcquisitionSucceeded() && (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(0)))
		setReadyForAcquisition();

	else if (acquisitionStatusControl_->withinTolerance(1) && acquireControl_->withinTolerance(1))
		setAcquiring();

	else if (acquisitionStatusControl_->withinTolerance(2) && acquireControl_->withinTolerance(0))
		setAcquisitionSucceeded();

	else if (acquisitionStatusControl_->withinTolerance(0) && acquireControl_->withinTolerance(0)){

		setAcquisitionSucceeded();
		setInitializationRequired();
	}

	else if (acquisitionStatusControl_->withinTolerance(0) || acquisitionStatusControl_->withinTolerance(5) || acquisitionStatusControl_->withinTolerance(10))
		setNotReadyForAcquisition();

	else if (isNotReadyForAcquisition() && requiresInitialization() && autoInitialize_)
		initialize();
}

double BioXASFourElementVortexDetector::elapsedTime() const
{
	return elapsedTime_.elapsed();
}

void BioXASFourElementVortexDetector::startElapsedTime()
{
	elapsedTime_.restart();
	elapsedTimeTimer_.start();
	emit elapsedTimeChanged(0.0);
}

void BioXASFourElementVortexDetector::stopElapsedTime()
{
	elapsedTimeTimer_.stop();
	emit elapsedTimeChanged(double(elapsedTime_.elapsed())/1000.0);
}

void BioXASFourElementVortexDetector::onElapsedTimerTimeout()
{
	emit elapsedTimeChanged(double(elapsedTime_.elapsed())/1000.0);
}

void BioXASFourElementVortexDetector::onStatusMessageChanged()
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

bool BioXASFourElementVortexDetector::cancelAcquisitionImplementation()
{
	acquireControl_->move(0);
	setAcquisitionCancelled();
	return true;
}

void BioXASFourElementVortexDetector::setFramesPerAcquisition(int count)
{
	if (framesPerAcquisition() != count)
		framesPerAcquisitionControl_->move(count);
}

void BioXASFourElementVortexDetector::setThreshold(int newThreshold)
{
	foreach (AMControl *control, thresholdControls_)
		control->move(newThreshold);
}

void BioXASFourElementVortexDetector::disarm()
{
	setInitializationRequired();
	setNotReadyForAcquisition();
	initializationControl_->move(0);
}

