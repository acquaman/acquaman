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


#include "AMXRFDetector.h"

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DSummingAB.h"
#include "analysis/AMnDDeadTimeAB.h"


AMXRFDetector::AMXRFDetector(const QString &name, const QString &description, QObject *parent)
	: AMDetector(name, description, parent)
{
	acquireTimeControl_ = 0; //NULL
	elapsedTimeControl_ = 0; //NULL
	acquisitionStatusControl_ = 0; //NULL
	acquireControl_ = 0; //NULL

	doDeadTimeCorrection_ = false;
	units_ = "Counts";

	regionOfInterestSignalMapper_ = new QSignalMapper(this);
	connect(regionOfInterestSignalMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(QObject*)));

	allControls_ = new AMControlSet(this);
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));
}

AMXRFDetector::~AMXRFDetector(){}

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

	else if (!icrControls_.isEmpty() && ocrControls_.isEmpty()){
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

	if (elapsedTimeControl_)
		connect(elapsedTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
}

void AMXRFDetector::buildSpectrumDataSources()
{
	foreach (AMReadOnlyPVControl *spectrum, spectraControls_){

		enabledElements_.append(rawSpectraSources_.size());
		rawSpectraSources_.append(new AM1DProcessVariableDataSource(spectrum->readPV(), spectrum->name(), this));
		rawSpectraSources_.last()->setDescription(rawSpectraSources_.last()->name());
	}
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

double AMXRFDetector::acquisitionTimeTolerance() const
{
	if (isConnected())
		return acquireTimeControl_->tolerance();

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
	bool stopSuccessful = acquireControl_->stop();	// This is safe because this will only be called if canCancel() returns true.
	setAcquisitionCancelled();
	return stopSuccessful;
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
	connect(newRegionOfInterest, SIGNAL(boundingRangeChanged(AMRange)), regionOfInterestSignalMapper_, SLOT(map()));
	regionOfInterestSignalMapper_->setMapping(newRegionOfInterest, newRegionOfInterest);
	regionsOfInterest_.append(newRegionOfInterest);
	newRegionOfInterest->setSpectrumSource(primarySpectrumDataSource_);
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

		disconnect(regionOfInterest, SIGNAL(boundingRangeChanged(AMRange)), regionOfInterestSignalMapper_, SLOT(map()));
		regionOfInterestSignalMapper_->removeMappings(regionOfInterest);
		removeRegionOfInterestImplementation(regionOfInterest);
		emit removedRegionOfInterest(regionOfInterest);
		regionOfInterest->deleteLater();
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

bool AMXRFDetector::data(double *outputValues) const
{
	return dataSource()->values(AMnDIndex(0), AMnDIndex(dataSource()->size(0)-1), outputValues);
}

void AMXRFDetector::onRegionOfInterestBoundingRangeChanged(QObject *region)
{
	// This is safe because only regions of interest will be passed into this method.
	emit regionOfInterestBoundingRangeChanged(qobject_cast<AMRegionOfInterest *>(region));
}

bool AMXRFDetector::isElementEnabled(int index) const
{
	return enabledElements_.contains(index);
}

void AMXRFDetector::enableElement(int elementId)
{
	if (!enabledElements_.contains(elementId)){

		enabledElements_.append(elementId);
		updatePrimarySpectrumSources();
		emit elementEnabled(elementId);
	}
}

void AMXRFDetector::disableElement(int elementId)
{
	if (enabledElements_.contains(elementId)){

		enabledElements_.removeAll(elementId);
		updatePrimarySpectrumSources();
		emit elementDisabled(elementId);
	}
}

void AMXRFDetector::updatePrimarySpectrumSources()
{
		QList<AMDataSource *> newSum;

		if (doDeadTimeCorrection_)
			foreach(int enabledElement, enabledElements_)
				newSum << analyzedSpectraSources_.at(enabledElement);

		else
			foreach(int enabledElement, enabledElements_)
				newSum << rawSpectraSources_.at(enabledElement);

		((AMAnalysisBlock *)primarySpectrumDataSource_)->setInputDataSources(newSum);
}
