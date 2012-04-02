/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "XRFDetector.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"

XRFDetector::XRFDetector(QString name, int elements, QString baseName, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	setElements(elements);

	wasConnected_ = false;
	timer_.setInterval(6000);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));

	// This makes me sad.  It destroys the extendability and generality of this class.
	if (elements == 1){

		startPV_ = new AMProcessVariable(baseName+QString(":mca1EraseStart"), true, this);
		stopPV_ = new AMProcessVariable(baseName+QString(":mca1Stop"), true, this);

		singleElSpectraPV_ = new AMProcessVariable(baseName+QString(":mca1Read.SCAN"), true, this);
		singleElStatusPV_ = new AMProcessVariable(baseName+QString(":mca1Status.SCAN"), true, this);
		fourElSpectraPV_ = 0;
		fourElStatusPV_ = 0;
		fourElAllPV_ = 0;

		connect(singleElSpectraPV_, SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
		connect(singleElStatusPV_, SIGNAL(valueChanged()), this, SLOT(onStatusUpdateRateInitialized()));

	}
	else if (elements == 4){

		startPV_ = new AMProcessVariable(baseName+QString(":EraseStart"), true, this);
		stopPV_ = new AMProcessVariable(baseName+QString(":StopAll"), true, this);

		singleElSpectraPV_ = 0;
		singleElStatusPV_ = 0;
		fourElSpectraPV_ = new AMProcessVariable(baseName+QString(":ReadDXPs.SCAN"), true, this);
		fourElStatusPV_ = new AMProcessVariable(baseName+QString(":StatusAll.SCAN"), true, this);
		fourElAllPV_ = new AMProcessVariable(baseName+QString(":ReadAll.SCAN"), true, this);

		connect(fourElSpectraPV_, SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
		connect(fourElStatusPV_, SIGNAL(valueChanged()), this, SLOT(onStatusUpdateRateInitialized()));
		connect(fourElAllPV_, SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
	}

	startPV_->disablePutCallbackMode(true);
	stopPV_->disablePutCallbackMode(true);

	connect(startPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
	connect(stopPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));

	// End of part that makes me sad.

	for (int i = 0; i < elements; i++){

//		if (i == 0){

			statusPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ACQG", true, this);
//			mcaUpdateRatePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".SCAN", true, this);
//			statusUpdateRatePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".SCAN", true, this);
			peakingTimePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".PKTIM", true, this);
			maximumEnergyPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".EMAX", true, this);
			integrationTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PRTM", true, this);
			liveTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PLTM", true, this);
			elapsedTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERTM", true, this);
//		}
//		else{

//			statusPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ACQG", false, this);
//			mcaUpdateRatePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".SCAN", false, this);
//			statusUpdateRatePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".SCAN", false, this);
//			peakingTimePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".PKTIM", false, this);
//			maximumEnergyPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".EMAX", false, this);
//			integrationTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PRTM", false, this);
//			liveTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PLTM", false, this);
//			elapsedTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERTM", false, this);
//		}

		icrPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".ICR", true, this);
		ocrPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".OCR", true, this);
		spectraPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1), true, this);

		statusPV_.at(i)->disablePutCallbackMode(true);
//		mcaUpdateRatePV_.at(i)->disablePutCallbackMode(true);
//		statusUpdateRatePV_.at(i)->disablePutCallbackMode(true);
		peakingTimePV_.at(i)->disablePutCallbackMode(true);
		maximumEnergyPV_.at(i)->disablePutCallbackMode(true);
		integrationTimePV_.at(i)->disablePutCallbackMode(true);
		liveTimePV_.at(i)->disablePutCallbackMode(true);
		elapsedTimePV_.at(i)->disablePutCallbackMode(true);
		icrPV_.at(i)->disablePutCallbackMode(true);
		ocrPV_.at(i)->disablePutCallbackMode(true);
		spectraPV_.at(i)->disablePutCallbackMode(true);

		connect(statusPV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));
//		connect(mcaUpdateRatePV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));
//		connect(statusUpdateRatePV_.at(i), SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(peakingTimePV_.at(i), SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(maximumEnergyPV_.at(i), SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(integrationTimePV_.at(i), SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(liveTimePV_.at(i), SIGNAL(writeReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(elapsedTimePV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(icrPV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(ocrPV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));
		connect(spectraPV_.at(i), SIGNAL(readReadyChanged(bool)), this, SLOT(isDetectorConnected()));

		spectrumDataSources_ << new AM1DProcessVariableDataSource(spectraPV_.at(i), QString("Element %1").arg(i+1), this);
		icrDataSources_ << new AM0DProcessVariableDataSource(icrPV_.at(i), QString("Input count rate %1").arg(i+1), this);
		ocrDataSources_ << new AM0DProcessVariableDataSource(ocrPV_.at(i), QString("Output count rate %1").arg(i+1), this);
		AMDeadTimeAB *corrected = new AMDeadTimeAB(QString("Corrected Element %1").arg(i+1), this);
		corrected->setInputDataSourcesImplementation(QList<AMDataSource *>() << (AMDataSource *)spectrumDataSources_.at(i) << (AMDataSource *)icrDataSources_.at(i) << (AMDataSource *)ocrDataSources_.at(i));
		correctedSpectrumDataSources_ << corrected;
	}

	connect(statusPV_.first(), SIGNAL(valueChanged()), this, SLOT(onStatusChanged()));
//	connect(mcaUpdateRatePV_.first(), SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
//	connect(statusUpdateRatePV_.first(), SIGNAL(valueChanged()), this, SLOT(onStatusUpdateRateInitialized()));
	connect(peakingTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onPeakingTimeChanged(double)));
	connect(maximumEnergyPV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
	connect(integrationTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onIntegrationTimeChanged(double)));
	connect(elapsedTimePV_.first(), SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
	connect(icrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));
	connect(ocrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));

	// This one is separate beccause this signal should only be called once.
	connect(spectraPV_.first(), SIGNAL(valueChanged()), this, SLOT(setChannelSize()));

	if (elements > 1){

		AM1DSummingAB *correctedSumAB = new AM1DSummingAB("Corrected Sum", this);
		correctedSumAB->setInputDataSourcesImplementation(correctedSpectrumDataSources_);
		correctedSpectrumDataSources_ << correctedSumAB;
	}

	createROIList(baseName);
	setSpectraRefreshRate(Slow);
}

XRFDetector::~XRFDetector()
{

}

void XRFDetector::createROIList(QString baseName)
{
	AMROI *roi;

	for (int i = 0; i < 32; i++){

		roi = new AMROI(baseName, elements_, i, this);
		roi->setScale(scale());
		roiList_ << roi;
		connect(roi, SIGNAL(roiHasValues()), this, SLOT(allRoisHaveValues()));
	}
}

double XRFDetector::deadTime()
{
	// For the single element, return the value.  For multi-element detectors, return the worst.
	if (elements_ == 1)
		return deadTimeAt(0);

	else {

		double dt = 0;

		for (int i = 0; i < elements_; i++) {

			if (dt < deadTimeAt(i))
				dt = deadTimeAt(i);
		}

		return dt;
	}
}

void XRFDetector::setChannelSize()
{
	setSize(AMnDIndex(spectraPV_.first()->count()));
	for (int i = 0; i < spectrumDataSources_.size(); i++)
		spectrumDataSources_.at(i)->setScale(scale());

	for (int i = 0; i < roiList_.size(); i++)
		roiList_.at(i)->setScale(scale());

	// Don't need to come here again because the size of the detector is static.
	disconnect(spectraPV_.first(), SIGNAL(valueChanged()), this, SLOT(setChannelSize()));
}

bool XRFDetector::setFromInfo(const AMDetectorInfo *info)
{
	const XRFDetectorInfo *detectorInfo = qobject_cast<const XRFDetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	for (int i = 0; i < elements_; i++){

		peakingTimePV_.at(i)->setValue(detectorInfo->peakingTime());
		maximumEnergyPV_.at(i)->setValue(detectorInfo->maximumEnergy());
		integrationTimePV_.at(i)->setValue(detectorInfo->integrationTime());
	}
	for (int i = 0; i < detectorInfo->roiInfoList()->count(); i++)
		roiList().at(i)->setRegion(detectorInfo->roiInfoList()->at(i));

	return true;
}

void XRFDetector::fromXRFInfo(const XRFDetectorInfo &info)
{
	for (int i = 0; i < elements_; i++){

		peakingTimePV_.at(i)->setValue(info.peakingTime());
		maximumEnergyPV_.at(i)->setValue(info.maximumEnergy());
		integrationTimePV_.at(i)->setValue(info.integrationTime());
	}
	for (int i = 0; i < info.roiInfoList()->count(); i++)
		roiList().at(i)->setRegion(info.roiInfoList()->at(i));
}

void XRFDetector::isDetectorConnected()
{
	wasConnected_ = isConnected();

	bool currentlyConnected = startPV_->writeReady() && stopPV_->writeReady();

	for (int i = 0; i < elements_; i++){

		currentlyConnected = currentlyConnected && statusPV_.at(i)->readReady()
//					&& mcaUpdateRatePV_.at(i)->readReady()
//					&& statusUpdateRatePV_.at(i)->writeReady()
					&& peakingTimePV_.at(i)->writeReady()
					&& maximumEnergyPV_.at(i)->writeReady()
					&& integrationTimePV_.at(i)->writeReady()
					&& liveTimePV_.at(i)->writeReady()
					&& elapsedTimePV_.at(i)->readReady()
					&& icrPV_.at(i)->readReady()
					&& ocrPV_.at(i)->readReady()
					&& spectraPV_.at(i)->readReady();
	}

	if (isConnected() != currentlyConnected){

		AMDetector::setConnected(currentlyConnected);
		emit connected(currentlyConnected);
	}
}

void XRFDetector::allRoisHaveValues()
{
	bool hasValues = true;

	for (int i = 0; i < roiList().size(); i++)
		hasValues = hasValues && roiList().at(i)->hasValues();

	if (hasValues){

		for (int i = 0; i < roiList_.size(); i++){

			connect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SIGNAL(roiUpdate(AMROI*)));
			connect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SLOT(onAMROIUpdate(AMROI*)));
		}

		emit roisHaveValues();
		timer_.start();
	}
	else{

		for (int i = 0; i < roiList_.size(); i++){

			disconnect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SIGNAL(roiUpdate(AMROI*)));
			disconnect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SLOT(onAMROIUpdate(AMROI*)));
		}
		timer_.stop();
	}
}

void XRFDetector::onUpdateTimer()
{
	bool resetAll = false;

	// Check to see if an ROI has been added or removed.
	int numRoi = 0;
	for (int i = 0; i < roiList().count(); i++)
		if (!roiList().at(i)->name().isEmpty())
			numRoi++;

	// One has been added or removed.
	if (numRoi != roiInfoList()->count())
		resetAll = true;

	// Check to see if the names match.  Only do this if we already don't have to reset anything.
	if (!resetAll){

		for (int i = 0; i < roiInfoList()->count(); i++)
			if (roiList().at(i)->name().compare(roiInfoList()->at(i).name()) != 0)
				resetAll = true;
	}

	// If a change has happened, do something about it.
	if (resetAll){

		roiInfoList()->clear();
		setROIList(*roiInfoList());
		emit externalRegionsOfInterestChanged();
	}
}

bool XRFDetector::addRegionOfInterest(XRFElement *el, QString line)
{
	// No more ROIs.
	if (roiInfoList()->count() == roiList().size())
		return false;

	AMROIInfo roi(el->lineEnergy(line), 0.04, scale(), el->symbol()+" "+GeneralUtilities::removeGreek(line));

	// Appending to the list means that the old size of the Info list is where the new values should be set in the ROI list.
	roiList_.at(roiInfoList()->count())->setRegion(roi);
	roiInfoList()->append(roi);
	setROIList(*roiInfoList());
	emit addedRegionOfInterest(roi);

	return true;
}

bool XRFDetector::addRegionOfInterest(XRFElement *el, QString line, bool propogateToDetector)
{
	if (propogateToDetector)
		return addRegionOfInterest(el, line);

	else {

		// No more ROIs.
		if (roiInfoList()->count() == roiList().size())
			return false;

		for (int i = 0; i < roiList_.size(); i++){

			if (roiList_.at(i)->name().compare(el->symbol()+" "+GeneralUtilities::removeGreek(line)) == 0){

				AMROIInfo roi(roiList_.at(i)->toInfo());

				if (((int)roi.energy()) == -1){

					double energy = el->lineEnergy(line);
					roi.setEnergy(energy);
					roiList_[i]->setEnergy(energy);
				}

				roiInfoList()->append(roi);
				setROIList(*roiInfoList());
				emit addedRegionOfInterest(roi);

				return true;
			}
		}

		return false;
	}
}

bool XRFDetector::removeRegionOfInterest(XRFElement *el, QString line)
{
	int indexOfRemoved = roiInfoList()->indexOf(el->symbol()+" "+GeneralUtilities::removeGreek(line));

	if (indexOfRemoved == -1)
		return false;

	// Slides all ROIs, after the removed ROI, down one place.
	for (int i = indexOfRemoved; i < roiInfoList()->count(); i++){

		if (i+1 == roiInfoList()->count())
			roiList_.at(i)->setRegion("", -1, -1);
		else
			roiList_.at(i)->setRegion(roiInfoList()->at(i+1));
	}

	emit removedRegionOfInterest(roiInfoList()->at(indexOfRemoved));
	roiInfoList()->remove(indexOfRemoved);
	setROIList(*roiInfoList());

	return true;
}

void XRFDetector::onAMROIUpdate(AMROI *roi)
{
	for (int i = 0; i < roiInfoList()->count(); i++){

		if (roiInfoList()->at(i).name() == roi->name())
			roiInfoList_[i].setValuesFrom(roi->toInfo());
	}
}

void XRFDetector::sortRegionsOfInterest()
{
	roiInfoList()->sort();
	setROIList(*roiInfoList());

	for (int i = 0; i < roiInfoList()->count(); i++)
		roiList_.at(i)->fromInfo(roiInfoList()->at(i));
}

void XRFDetector::clearRegionsOfInterest()
{
	for (int i = 0; i < roiList().count(); i++)
		roiList_.at(i)->setRegion("", -1, -1);

	roiInfoList()->clear();
	setROIList(*roiInfoList());
}

void XRFDetector::enableElement(int id)
{
	activeElements_[id] = true;

	QList<AMDataSource *> newSum;
	for (int i = 0; i < elements_; i++)
		if (activeElements_.at(i))
			newSum << correctedSpectrumDataSources_.at(i);

	((AM1DSummingAB *)correctedSumDataSource())->setInputDataSourcesImplementation(newSum);
}

void XRFDetector::disableElement(int id)
{
	activeElements_[id] = false;

	QList<AMDataSource *> newSum;
	for (int i = 0; i < elements_; i++)
		if (activeElements_.at(i))
			newSum << correctedSpectrumDataSources_.at(i);

	((AM1DSummingAB *)correctedSumDataSource())->setInputDataSourcesImplementation(newSum);
}

QVector<int> XRFDetector::spectraValues(int index)
{
	if (index < elements_ && index >= 0)
		return spectraPV_.at(index)->lastIntegerValues();

	return QVector<int>();
}

double XRFDetector::deadTimeAt(int index)
{
	if (icrPV_.at(index)->getInt() == 0)
		return 0;

	else if (index < elements_ && index >= 0){

		double deadTime = 100*(1 - ocrPV_.at(index)->getDouble()/icrPV_.at(index)->getDouble());

		return deadTime >= 0 ? deadTime : 0;
	}

	return -1;
}

void XRFDetector::setTime(double time)
{
	setIntegrationTime(time);
	for (int i = 0; i < elements_; i++){

		integrationTimePV_.at(i)->setValue(time);
		liveTimePV_.at(i)->setValue(0.0);
	}
}

void XRFDetector::setMaximumEnergyControl(double energy)
{
	setMaximumEnergy(energy);
	for (int i = 0; i < elements_; i++)
		maximumEnergyPV_.at(i)->setValue(energy/1000);
}

void XRFDetector::setPeakingTimeControl(double time)
{
	setPeakingTime(time);
	for (int i = 0; i < elements_; i++)
		peakingTimePV_.at(i)->setValue(time);
}
