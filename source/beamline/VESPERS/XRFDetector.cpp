#include "XRFDetector.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"

XRFDetector::XRFDetector(QString name, int elements, QString baseName, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(elements);

	wasConnected_ = false;
	detectorConnected_ = false;
	timer_.setInterval(6000);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));

	for (int i = 0; i < elements; i++){

		if (i == 0){

			statusPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ACQG", true, this);
			refreshRatePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".SCAN", true, this);
			peakingTimePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".PKTIM", true, this);
			maximumEnergyPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".EMAX", true, this);
			integrationTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PRTM", true, this);
			liveTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PLTM", true, this);
			elapsedTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERTM", true, this);
			startPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERST", true, this);
			stopPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".STOP", true, this);
		}
		else{

			statusPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ACQG", false, this);
			refreshRatePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".SCAN", false, this);
			peakingTimePV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".PKTIM", false, this);
			maximumEnergyPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".EMAX", false, this);
			integrationTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PRTM", false, this);
			liveTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".PLTM", false, this);
			elapsedTimePV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERTM", false, this);
			startPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".ERST", false, this);
			stopPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1)+".STOP", false, this);
		}

		icrPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".ICR", true, this);
		ocrPV_ << new AMProcessVariable(baseName+QString(":dxp%1").arg(i+1)+".OCR", true, this);
		spectraPV_ << new AMProcessVariable(baseName+QString(":mca%1").arg(i+1), true, this);

		statusPV_.at(i)->disablePutCallbackMode(true);
		refreshRatePV_.at(i)->disablePutCallbackMode(true);
		peakingTimePV_.at(i)->disablePutCallbackMode(true);
		maximumEnergyPV_.at(i)->disablePutCallbackMode(true);
		integrationTimePV_.at(i)->disablePutCallbackMode(true);
		liveTimePV_.at(i)->disablePutCallbackMode(true);
		elapsedTimePV_.at(i)->disablePutCallbackMode(true);
		icrPV_.at(i)->disablePutCallbackMode(true);
		ocrPV_.at(i)->disablePutCallbackMode(true);
		startPV_.at(i)->disablePutCallbackMode(true);
		stopPV_.at(i)->disablePutCallbackMode(true);
		spectraPV_.at(i)->disablePutCallbackMode(true);

		connect(statusPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(refreshRatePV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(peakingTimePV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(maximumEnergyPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(integrationTimePV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(liveTimePV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(elapsedTimePV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(icrPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(ocrPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(startPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(stopPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));
		connect(spectraPV_.at(i), SIGNAL(connected()), this, SLOT(isDetectorConnected()));

		// This one is separate beccause this signal should only be called once.
		connect(spectraPV_.first(), SIGNAL(valueChanged()), this, SLOT(setChannelSize()));

		spectrumDataSources_ << new AM1DProcessVariableDataSource(spectraPV_.at(i), QString("Element %1").arg(i+1), this);
		icrDataSources_ << new AM0DProcessVariableDataSource(icrPV_.at(i), QString("Input count rate %1").arg(i+1), this);
		ocrDataSources_ << new AM0DProcessVariableDataSource(ocrPV_.at(i), QString("Output count rate %1").arg(i+1), this);
		AMDeadTimeAB *corrected = new AMDeadTimeAB(QString("Corrected Element %1").arg(i+1), this);
		corrected->setInputDataSourcesImplementation(QList<AMDataSource *>() << (AMDataSource *)spectrumDataSources_.at(i) << (AMDataSource *)icrDataSources_.at(i) << (AMDataSource *)ocrDataSources_.at(i));
		correctedSpectrumDataSources_ << corrected;
	}

	if (elements > 1){

		AM1DSummingAB *correctedSumAB = new AM1DSummingAB("Corrected Sum", this);
		correctedSumAB->setInputDataSourcesImplementation(correctedSpectrumDataSources_);
		correctedSpectrumDataSources_ << correctedSumAB;
	}

	createROIList(baseName);
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
	wasConnected_ = detectorConnected_;

	bool connected = true;

	for (int i = 0; i < elements_; i++){

		connected = connected && statusPV_.at(i)->isConnected()
					&& refreshRatePV_.at(i)->isConnected()
					&& peakingTimePV_.at(i)->isConnected()
					&& maximumEnergyPV_.at(i)->isConnected()
					&& integrationTimePV_.at(i)->isConnected()
					&& liveTimePV_.at(i)->isConnected()
					&& elapsedTimePV_.at(i)->isConnected()
					&& icrPV_.at(i)->isConnected()
					&& ocrPV_.at(i)->isConnected()
					&& startPV_.at(i)->isConnected()
					&& stopPV_.at(i)->isConnected()
					&& spectraPV_.at(i)->isConnected();
	}

	if (detectorConnected_ != connected){

		detectorConnected_ = connected;
		onConnectedChanged(connected);
		emit detectorConnected(detectorConnected_);
	}
}

void XRFDetector::onConnectedChanged(bool isConnected)
{
	// Only connecting the first element because all the other elements act in unison.  This will minimize signal traffic on many-element detectors.
	if (isConnected){

		connect(statusPV_.first(), SIGNAL(valueChanged()), this, SLOT(onStatusChanged()));
		connect(refreshRatePV_.first(), SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
		connect(peakingTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onPeakingTimeChanged(double)));
		connect(maximumEnergyPV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
		connect(integrationTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onIntegrationTimeChanged(double)));
		connect(elapsedTimePV_.first(), SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
		connect(icrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));
		connect(ocrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));
	}
	else{

		disconnect(statusPV_.first(), SIGNAL(valueChanged()), this, SLOT(onStatusChanged()));
		disconnect(refreshRatePV_.first(), SIGNAL(valueChanged(int)), this, SLOT(onRefreshRateChanged(int)));
		disconnect(peakingTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onPeakingTimeChanged(double)));
		disconnect(maximumEnergyPV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));
		disconnect(integrationTimePV_.first(), SIGNAL(valueChanged(double)), this, SLOT(onIntegrationTimeChanged(double)));
		disconnect(elapsedTimePV_.first(), SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
		disconnect(icrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));
		disconnect(ocrPV_.first(), SIGNAL(valueChanged()), this, SIGNAL(deadTimeChanged()));
	}
}

void XRFDetector::allRoisHaveValues()
{
	bool hasValues = true;

	for (int i = 0; i < roiList().size(); i++)
		hasValues = hasValues && roiList().at(i)->hasValues();

	if (hasValues){

		for (int i = 0; i < roiList_.size(); i++)
			connect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SIGNAL(roiUpdate(AMROI*)));
		emit roisHaveValues();
		timer_.start();
	}
	else{

		for (int i = 0; i < roiList_.size(); i++)
			disconnect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SIGNAL(roiUpdate(AMROI*)));
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

void XRFDetector::sort()
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
	if (index < elements_ && index >= 0)
		return 100*(1 - ocrPV_.at(index)->getDouble()/icrPV_.at(index)->getDouble());

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
