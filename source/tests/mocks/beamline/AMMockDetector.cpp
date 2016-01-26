#include "AMMockDetector.h"

#include <QTimer>
AMMockDetector::AMMockDetector(const QString& name,
                               const QString& description,
                               AMDetectorDefinitions::ReadMethod readMethod,
	                           QObject *parent) :
    AMDetector(name, description, parent)
{

	readMode_ = AMDetectorDefinitions::SingleRead;
	acquisitionTime_ = 1.0;
	acquisitionTimeTolerance_ = 0.001;
	readMethod_ = readMethod;
	setConnected(true);
	setReadyForAcquisition();
}

double AMMockDetector::acquisitionTime() const
{
	return acquisitionTime_;
}

double AMMockDetector::acquisitionTimeTolerance() const
{
	return acquisitionTimeTolerance_;
}

bool AMMockDetector::supportsSynchronizedDwell() const
{
	return readMethod_ != AMDetectorDefinitions::ImmediateRead;
}

QString AMMockDetector::synchronizedDwellKey() const
{
	return name() + "dwell key";
}

AMDetectorDefinitions::ReadMethod AMMockDetector::readMethod() const
{
	return readMethod_;
}

AMDetectorDefinitions::ReadMode AMMockDetector::readMode() const
{
	return readMode_;
}

AMNumber AMMockDetector::reading(const AMnDIndex &indexes) const
{
	// We want an "invalid" AMnDIndex for this 0D detector
	if (indexes.isValid()) {
		return AMNumber(AMNumber::DimensionError);
	}

	return generateRandomNumber();
}

AMNumber AMMockDetector::singleReading() const
{
	return generateRandomNumber();
}

bool AMMockDetector::data(double *outputValues) const
{
	if(!outputValues) {

		return false;
	}

	outputValues[0] = generateRandomNumber();

	return true;
}

AMDataSource * AMMockDetector::dataSource() const
{
	return 0;
}

bool AMMockDetector::setAcquisitionTime(double seconds)
{
	if(readMethod_ == AMDetectorDefinitions::ImmediateRead) {
		emit acquisitionTimeChanged(seconds); // Have to emit this or action fails.
		return false;
	}

	acquisitionTime_ = seconds;
	emit acquisitionTimeChanged(seconds);
	return true;
}

bool AMMockDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool AMMockDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	if(readMode != AMDetectorDefinitions::SingleRead) {
		return false;
	}

	if(readMethod_ == AMDetectorDefinitions::ImmediateRead) {
		setAcquiring();
		setAcquisitionSucceeded();
		setReadyForAcquisition();
	} else if (readMethod_ == AMDetectorDefinitions::RequestRead
	           || readMethod_ == AMDetectorDefinitions::WaitRead) {

		setAcquiring();
		QTimer::singleShot(int(acquisitionTime_ * 1000), this, SLOT(onRequestTimerTimedOut()));
	}
	return true;
}

bool AMMockDetector::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

double AMMockDetector::generateRandomNumber() const
{
	double randomValue = (double)qrand() / RAND_MAX;
	return randomValue * (1500);
}

void AMMockDetector::onRequestTimerTimedOut()
{
	setAcquisitionSucceeded();
	setReadyForAcquisition();
}

