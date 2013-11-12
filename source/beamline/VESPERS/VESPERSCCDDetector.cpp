#include "VESPERSCCDDetector.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/VESPERS/VESPERSBeamlineSetStringAction.h"
#include "beamline/AMBeamline.h"

#include <QStringBuilder>

VESPERSCCDDetector::VESPERSCCDDetector(const QString &name, const QString &description, QObject *parent)
	: AMDetector(name, description, parent)
{
	units_ = "Counts";

	allControls_ = new AMControlSet(this);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
	connect(&elapsedTimer_, SIGNAL(timeout()), this, SLOT(onElapsedTimeChanged()));
}

void VESPERSCCDDetector::allControlsCreated()
{
	allControls_->addControl(acquireControl_);
	allControls_->addControl(acquireTimeControl_);
	allControls_->addControl(acquisitionStatusControl_);
	allControls_->addControl(ccdFilePathControl_);
	allControls_->addControl(ccdFileBaseNameControl_);
	allControls_->addControl(ccdFileNumberControl_);

	connect(acquireTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(ccdFilePathControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFilePathChanged()));
	connect(ccdFileBaseNameControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFileBaseNameChanged()));
	connect(ccdFileNumberControl_, SIGNAL(valueChanged(double)), this, SLOT(onCCDFileNumberChanged()));
}

void VESPERSCCDDetector::onControlsConnected(bool connected)
{
	setConnected(connected);

	if (connected)
		setReadyForAcquisition();

	else if (!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void VESPERSCCDDetector::onControlsTimedOut()
{
	setConnected(false);
}

void VESPERSCCDDetector::onStatusControlChanged()
{
	if (acquisitionStatusControl_->withinTolerance(1))
		setAcquiring();

	else if (acquisitionStatusControl_->withinTolerance(0)){

		if (isAcquiring())
			setAcquisitionSucceeded();

		if (!isConnected() && !isNotReadyForAcquisition())
			setNotReadyForAcquisition();

		else if (isConnected() && !isReadyForAcquisition())
			setReadyForAcquisition();
	}
}

AMBeamlineActionItem *VESPERSCCDDetector::createFilePathAction(const QString &path)
{
//	if (!ccdFilePathControl_->isConnected())
//		return 0;

//	return new VESPERSBeamlineSetStringAction(ccdFilePathControl_, path);
	return 0;
}

AMBeamlineActionItem *VESPERSCCDDetector::createFileNameAction(const QString &name)
{
//	if (!ccdFileBaseNameControl_->isConnected())
//		return 0;

//	return new VESPERSBeamlineSetStringAction(ccdFileBaseNameControl_, name);
}

AMBeamlineActionItem *VESPERSCCDDetector::createFileNumberAction(int number)
{
//	if (!ccdFileNumberControl_->isConnected())
//		return 0;

//	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(ccdFileNumberControl_);
//	action->setSetpoint(number);

//	return action;
}

bool VESPERSCCDDetector::sharesDetectorTriggerSource()
{
	return currentlySynchronizedDwell();
}

AMDetectorTriggerSource* VESPERSCCDDetector::detectorTriggerSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();

	return 0;
}

AMDetectorDwellTimeSource* VESPERSCCDDetector::detectorDwellTimeSource()
{
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();

	return 0;
}

bool VESPERSCCDDetector::lastContinuousReading(double *outputValues) const
{
	Q_UNUSED(outputValues)

	return false;
}

bool VESPERSCCDDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)

	return false;
}

bool VESPERSCCDDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool VESPERSCCDDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	if (!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	elapsedTimer_.start();
	elapsedTime_.restart();
	AMControl::FailureExplanation failureExplanation = acquireControl_->move(1);
	return failureExplanation == AMControl::NoFailure;
}

bool VESPERSCCDDetector::cleanupImplementation()
{
	elapsedTimer_.stop();
	setCleanedUp();
	return true;
}

bool VESPERSCCDDetector::cancelAcquisitionImplementation()
{
	elapsedTimer_.stop();
	bool stopSuccessful = acquireControl_->stop();
	setAcquisitionCancelled();
	return stopSuccessful;
}

bool VESPERSCCDDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	if(!acquireTimeControl_->withinTolerance(seconds))
		acquireTimeControl_->move(seconds);

	return true;
}

double VESPERSCCDDetector::acquisitionTime() const
{
	if (isConnected())
		return acquireTimeControl_->value();

	return -1;
}

QString VESPERSCCDDetector::ccdFilePath() const
{
	const AMProcessVariable *pv = ccdFilePathControl_->readPV();
	return VESPERS::pvToString(pv);
}

QString VESPERSCCDDetector::ccdFileBaseName() const
{
	const AMProcessVariable *pv = ccdFileBaseNameControl_->readPV();
	return VESPERS::pvToString(pv);
}

int VESPERSCCDDetector::ccdFileNumber() const
{
	return int(ccdFileNumberControl_->value());
}

void VESPERSCCDDetector::onCCDFilePathChanged()
{
	const AMProcessVariable *pv = ccdFilePathControl_->readPV();
	emit ccdPathChanged(VESPERS::pvToString(pv));
}

void VESPERSCCDDetector::onCCDFileBaseNameChanged()
{
	const AMProcessVariable *pv = ccdFileBaseNameControl_->readPV();
	emit ccdNameChanged(VESPERS::pvToString(pv));
}

void VESPERSCCDDetector::onCCDFileNumberChanged()
{
	emit ccdNumberChanged(int(ccdFileNumberControl_->value()));
}

void VESPERSCCDDetector::setCCDFilePath(QString path)
{
	AMProcessVariable *pv = ccdFilePathControl_->writePV();
	VESPERS::stringToPV(pv, path);
}

void VESPERSCCDDetector::setCCDFileBaseName(QString name)
{
	AMProcessVariable *pv = ccdFileBaseNameControl_->writePV();
	VESPERS::stringToPV(pv, name);
}

void VESPERSCCDDetector::setCCDNumber(int number)
{
	ccdFileNumberControl_->move(double(number));
}

void VESPERSCCDDetector::onElapsedTimeChanged()
{
	emit elapsedTimeChanged(elapsedTime_.elapsed()/1000.0);
}
