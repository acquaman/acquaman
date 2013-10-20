#include "AMXRFDetector.h"

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DSummingAB.h"
#include "analysis/AMnDDeadTimeAB.h"

AMXRFDetector::AMXRFDetector(const QString &name, const QString &description, QObject *parent)
	: AMDetector(name, description, parent)
{
	doDeadTimeCorrection_ = false;
	units_ = "Counts";

	allControls_ = new AMControlSet(this);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
}

void AMXRFDetector::allControlsCreated()
{
	allControls_->addControl(acquireTimeControl_);
	allControls_->addControl(acquisitionStatusControl_);
	allControls_->addControl(acquireControl_);

	if (supportsElapsedTime())
		allControls_->addControl(elapsedTimeControl_);

	for (int i = 0, size = spectraControls_.size(); i < size; i++)
		allControls_->addControl(spectraControls_.at(i));

	for (int i = 0, size = icrControls_.size(); i < size; i++)
		allControls_->addControl(icrControls_.at(i));

	for (int i = 0, size = ocrControls_.size(); i < size; i++)
		allControls_->addControl(ocrControls_.at(i));

	// There should always be at least one and even if there isn't, this will be fine.
	buildSpectrumDataSources();

	if ((spectraControls_.size() == icrControls_.size()) && (icrControls_.size() == ocrControls_.size()) && !icrControls_.isEmpty()){

		doDeadTimeCorrection_ = true;
		buildDeadTimeDataSources();
		buildAllAnalysisBlocks();
	}

	else if (icrControls_.isEmpty() && ocrControls_.isEmpty()){

		doDeadTimeCorrection_ = false;
		buildAllAnalysisBlocks();
	}

	else {

		AMErrorMon::error(this,
						  AMXRFDETECTOR_SPECTRUMSIZE_DEADTIMESIZE_MISMATCH,
						  "There is a discrepancy between the number of spectra and dead time channels.  The expected behaviour of the detector may now be undefined.");
	}

	connect(acquisitionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlChanged()));
	connect(acquireTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(acquisitionTimeChanged(double)));
	connect(elapsedTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
}

void AMXRFDetector::buildSpectrumDataSources()
{
	foreach (AMReadOnlyPVControl *spectrum, spectraControls_)
		rawSpectraSources_.append(new AM1DProcessVariableDataSource(spectrum->readPV(), spectrum->name(), this));
}

void AMXRFDetector::buildDeadTimeDataSources()
{
	foreach (AMReadOnlyPVControl *icr, icrControls_){

		AMDataSource *source = new AM0DProcessVariableDataSource(icr->readPV(), icr->name(), this);
		icrSources_.append(source);
		connect(source->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SIGNAL(deadTimeChanged()));
	}

	foreach (AMReadOnlyPVControl *ocr, ocrControls_){

		AMDataSource *source = new AM0DProcessVariableDataSource(ocr->readPV(), ocr->name(), this);
		ocrSources_.append(source);
		connect(source->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SIGNAL(deadTimeChanged()));
	}
}

void AMXRFDetector::buildAllAnalysisBlocks()
{
	if (doDeadTimeCorrection_){

		for (int i = 0, size = rawSpectraSources_.size(); i < size; i++){

			AMnDDeadTimeAB *newDeadTimeAnalysisBlock = new AMnDDeadTimeAB(QString("Corrected %1").arg(rawSpectraSources_.at(i)->name()), this);
			newDeadTimeAnalysisBlock->setInputDataSources(QList<AMDataSource *>() << rawSpectraSources_.at(i) << icrSources_.at(i) << ocrSources_.at(i));
			analyzedSpectraSources_.append(newDeadTimeAnalysisBlock);
		}

		if (analyzedSpectraSources_.size() > 1){

			AM1DSummingAB *spectraSum = new AM1DSummingAB("Summed Spectrum", this);
			spectraSum->setInputDataSources(analyzedSpectraSources_);
			analyzedSpectraSources_.append(spectraSum);

			primarySpectrumDataSource_ = spectraSum;
		}

		else
			primarySpectrumDataSource_ = analyzedSpectraSources_.first();
	}

	else {

		if (rawSpectraSources_.size() == 1)
			primarySpectrumDataSource_ = rawSpectraSources_.first();

		else {

			AM1DSummingAB *spectraSum = new AM1DSummingAB("Summed Spectrum", this);
			spectraSum->setInputDataSources(rawSpectraSources_);
			analyzedSpectraSources_.append(spectraSum);

			primarySpectrumDataSource_ = spectraSum;
		}
	}
}

double AMXRFDetector::acquisitionTime() const
{
	if (isConnected())
		return acquireTimeControl_->value();

	return -1;
}

double AMXRFDetector::elapsedTime() const
{
	if (isConnected())
		return elapsedTimeControl_->value();

	return -1;
}

double AMXRFDetector::deadTime() const
{
	double maximumDeadTime = deadTimeAt(0);

	for (int i = 1, size = icrSources_.size(); i < size; i++)
		if (maximumDeadTime < deadTimeAt(i))
			maximumDeadTime = deadTimeAt(i);

	return maximumDeadTime;
}

double AMXRFDetector::deadTimeAt(int index) const
{
	double inputCounts = icrSources_.at(index)->value(AMnDIndex());
	double outputCounts = ocrSources_.at(index)->value(AMnDIndex());

	if (inputCounts == 0 || outputCounts == 0 || inputCounts < outputCounts)
		return 0;

	return 1 - outputCounts/inputCounts;
}

AMNumber AMXRFDetector::reading(const AMnDIndex &indexes) const
{
	if ( (!isConnected()) || (indexes.rank() != 1) || (indexes.i() >= size(0)) || indexes.i() < 0)
		return AMNumber(AMNumber::DimensionError);

	return dataSource()->value(indexes);
}

AMNumber AMXRFDetector::singleReading() const
{
	if (!isConnected())
		return AMNumber(AMNumber::Null);

	int detectorSize = size(0);
	QVector<double> spectrum = QVector<double>(detectorSize);

	if (!dataSource()->values(0, detectorSize-1, spectrum.data()))
		return AMNumber(AMNumber::Null);

	double value = 0;

	for (int i = 0; i < detectorSize; i++)
		value += spectrum.at(i);

	return value;
}

bool AMXRFDetector::reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const
{
	// No need for checking valid indices because that will be handled inside of the values() implementation of the data source.
	return dataSource()->values(startIndex, endIndex, outputValues);
}

bool AMXRFDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool AMXRFDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	if (!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	AMControl::FailureExplanation failureExplanation = acquireControl_->move(1);
	return failureExplanation == AMControl::NoFailure;
}

bool AMXRFDetector::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

bool AMXRFDetector::cancelAcquisitionImplementation()
{
	acquireControl_->stop();	// This is safe because this will only be called if canCancel() returns true.
	setAcquisitionCancelled();
	return true;
}

bool AMXRFDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	if(!acquireTimeControl_->withinTolerance(seconds))
		acquireTimeControl_->move(seconds);

	return true;
}

void AMXRFDetector::onControlsConnected(bool connected)
{
	setConnected(connected);

	if (connected)
		setReadyForAcquisition();

	else if (!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}

void AMXRFDetector::onControlsTimedOut()
{
	setConnected(false);
}

void AMXRFDetector::onStatusControlChanged()
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

void AMXRFDetector::addRegionOfInterest(const AMEmissionLine &emissionLine)
{
	double energy = emissionLine.energy();
	double halfWidth = (2.75/2)*sqrt(energy)/energy;
	AMRegionOfInterest *region = new AMRegionOfInterest(emissionLine.name(), energy, AMRange(energy*(1-halfWidth), energy*(1+halfWidth)), this);

	addRegionOfInterest(region);
}

void AMXRFDetector::addRegionOfInterest(AMRegionOfInterest *newRegionOfInterest)
{
	regionsOfInterest_.append(newRegionOfInterest);
	addRegionOfInterestImplementation(newRegionOfInterest);
	emit addedRegionOfInterest(newRegionOfInterest);
}

void AMXRFDetector::removeRegionOfInterest(const AMEmissionLine &emissionLine)
{
	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		if (emissionLine.name() == region->name())
			removeRegionOfInterest(region);
}

void AMXRFDetector::removeRegionOfInterest(AMRegionOfInterest *regionOfInterest)
{
	if (regionsOfInterest_.removeOne(regionOfInterest)){

		removeRegionOfInterestImplementation(regionOfInterest);
		emit removedRegionOfInterest(regionOfInterest);
		delete regionOfInterest;
	}
}

void AMXRFDetector::removeAllRegionsOfInterest()
{
	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		removeRegionOfInterest(region);
}

AMRegionOfInterest *AMXRFDetector::regionOfInterest(const AMEmissionLine &emissionLine) const
{
	foreach (AMRegionOfInterest *region, regionsOfInterest_)
		if (region->name() == emissionLine.name())
			return region;

	return 0;
}

void AMXRFDetector::addRegionOfInterestImplementation(AMRegionOfInterest *newRegionOfInterest)
{
	// Base class does nothing extra.
	Q_UNUSED(newRegionOfInterest);
}

void AMXRFDetector::removeRegionOfInterestImplementation(AMRegionOfInterest *regionOfInterest)
{
	// Base class does nothing extra.
	Q_UNUSED(regionOfInterest);
}
