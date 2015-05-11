#include "AM1DControlDetectorEmulator.h"

AM1DControlDetectorEmulator::AM1DControlDetectorEmulator(const QString &name, const QString &description, int size, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, QObject *parent) :
	AMDetector(name, description, parent)
{
	waitingForNewData_ = false;
	waitingForStatusChange_ = false;

	statusAcquiringValue_ = statusAcquiringValue;
	statusNotAcquiringValue_ = statusNotAcquiringValue;

	readMethod_ = readMethod;

	setIsVisible(false);
	setHiddenFromUsers(true);

	allControls_ = new AMControlSet(this);
	control_ = qobject_cast<AMReadOnlyPVControl *>(control);
	statusControl_ = qobject_cast<AMReadOnlyPVControl *>(statusControl);

	if(control_)
		allControls_->addControl(control_);

	if(statusControl_)
		allControls_->addControl(statusControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	if(control_)
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)));

	if(statusControl_)
		connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlValueChanged(double)));

	axes_ << AMAxisInfo(control_->name(), size, description);
}

int AM1DControlDetectorEmulator::size(int axisNumber) const
{
	if (axisNumber == 0)
		return control_->readPV()->lastIntegerValues().size();

	return -1;
}

AMNumber AM1DControlDetectorEmulator::reading(const AMnDIndex &indexes) const
{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an AMnDIndex with rank 1 for this 1D detector
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	return control_->readPV()->lastIntegerValues().at(indexes.i());
}

AMNumber AM1DControlDetectorEmulator::singleReading() const
{
	return 0;
}

bool AM1DControlDetectorEmulator::data(double *outputValues) const
{
	QVector<int> controlData = control_->readPV()->lastIntegerValues();
	QVector<double> detectorData = QVector<double>(controlData.size());

	for (int i = 0, size = detectorData.size(); i < size; i++)
		detectorData[i] = controlData.at(i);

	memcpy(outputValues, detectorData.constData(), detectorData.size()*sizeof(double));
	return true;
}

AMAction3* AM1DControlDetectorEmulator::createTriggerAction(AMDetectorDefinitions::ReadMode readMode)
{
	if(readMethod() != AMDetectorDefinitions::RequestRead)
		return 0;

	return AMDetector::createTriggerAction(readMode);
}

void AM1DControlDetectorEmulator::onControlsConnected(bool connected)
{
	if(connected)
		setConnected(connected);

	else
		setConnected(false);

	checkReadyForAcquisition();
}

void AM1DControlDetectorEmulator::onControlsTimedOut()
{
	setConnected(false);
}

void AM1DControlDetectorEmulator::onControlValueChanged(double value)
{
	Q_UNUSED(value)

	emit newValuesAvailable();

	if(waitingForNewData_){

		waitingForNewData_ = false;
		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

void AM1DControlDetectorEmulator::onStatusControlValueChanged(double value)
{
	Q_UNUSED(value)

	if(waitingForStatusChange_ && statusControl_ && statusControl_->withinTolerance(statusNotAcquiringValue_)){

		waitingForStatusChange_ = false;
		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

bool AM1DControlDetectorEmulator::initializeImplementation()
{
	setInitialized();
	return true;
}

bool AM1DControlDetectorEmulator::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	setAcquiring();

	if(readMethod() == AMDetectorDefinitions::ImmediateRead){

		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}

	else if(readMethod() == AMDetectorDefinitions::WaitRead)
		waitingForNewData_ = true;

	else if((readMethod() == AMDetectorDefinitions::RequestRead) && statusControl_ )
		waitingForStatusChange_ = true;

	return true;
}

bool AM1DControlDetectorEmulator::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

void AM1DControlDetectorEmulator::checkReadyForAcquisition()
{
	if(isConnected())
		setReadyForAcquisition();

	else
		setNotReadyForAcquisition();
}
