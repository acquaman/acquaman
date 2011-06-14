#include "XRFDetector.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"

XRFDetector::XRFDetector(QString name, int elements, AMControl *status, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControlSet *deadTime, AMControlSet *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(elements);

	wasConnected_ = false;
	detectorConnected_ = false;
	timer_.setInterval(6000);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));

	statusControl_ = status;
	refreshRateControl_ = refreshRate;
	peakingTimeControl_ = peakingTime;
	maximumEnergyControl_ = maximumEnergy;
	integrationTimeControl_ = integrationTime;
	liveTimeControl_ = liveTime;
	elapsedTimeControl_ = elapsedTime;
	startControl_ = start;
	stopControl_ = stop;

	deadTimeControl_ = new AMControlSet(this);
	for (int i = 0; i < deadTime->count(); i++)
		deadTimeControl_->addControl(deadTime->at(i));
	spectraControl_ = new AMControlSet(this);
	for (int i = 0; i < spectra->count(); i++)
		spectraControl_->addControl(spectra->at(i));

	readingControls_ = new AMControlSet(this);
	settingsControls_ = new AMControlSet(this);

	readingControls_->addControl(status);
	readingControls_->addControl(elapsedTime);
	for (int i = 0; i < deadTimeControl_->count(); i++)
		readingControls_->addControl(deadTimeControl_->at(i));
	for (int i = 0; i < spectraControl_->count(); i++)
		readingControls_->addControl(spectraControl_->at(i));

	settingsControls_->addControl(refreshRateControl_);
	settingsControls_->addControl(peakingTimeControl_);
	settingsControls_->addControl(maximumEnergyControl_);
	settingsControls_->addControl(integrationTimeControl_);
	settingsControls_->addControl(liveTimeControl_);
	settingsControls_->addControl(startControl_);
	settingsControls_->addControl(stopControl_);

	connect(readingControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SIGNAL(maximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(integrationTimeChanged(double)));
	connect(elapsedTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
	connect(refreshRateControl_, SIGNAL(valueChanged(double)), this, SIGNAL(refreshRateChanged(double)));
	connect(deadTimeControl_, SIGNAL(controlSetValuesChanged()), this, SIGNAL(deadTimeChanged()));
	connect(spectraControl_, SIGNAL(controlSetValuesChanged()), this, SIGNAL(spectraChanged()));

	connect(statusControl_, SIGNAL(valueChanged(double)), this, SIGNAL(statusChanged()));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setTime(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setPeakingTime(double)));
	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(setMaximumEnergy(double)));
	connect(spectraControl_, SIGNAL(controlSetValuesChanged()), this, SLOT(setChannelSize()));
	//connect(spectraControl_.at(0), SIGNAL(valueChanged()), this, SLOT(setChannelSize()));

	AMReadOnlyPVControl *spectrum;
	AMReadOnlyPVControl *deadTimePV;

	for (int i = 0; i < elements; i++){

		spectrum = qobject_cast<AMReadOnlyPVControl *>(spectraControl_->at(i));
		deadTimePV = qobject_cast<AMReadOnlyPVControl *>(deadTime->at(i));

		if (spectrum != 0 && deadTimePV != 0){
			spectrumDataSources_ << new AM1DProcessVariableDataSource(spectrum->readPV(), QString("Element %1").arg(i+1), this);
			deadTimeDataSources_ << new AM0DProcessVariableDataSource(deadTimePV->readPV(), QString("Dead time %1").arg(i+1), this);
			AMDeadTimeAB *corrected = new AMDeadTimeAB(QString("Corrected Element %1").arg(i+1), this);
			QList<AMDataSource *> corrList;
			corrList << (AMDataSource *)spectrumDataSources_.first() << (AMDataSource *)deadTimeDataSources_.first();
			corrected->setInputDataSourcesImplementation(corrList);
			correctedSpectrumDataSources_ << corrected;
		}
	}

	AM1DSummingAB *correctedSumAB = new AM1DSummingAB("Corrected Sum", this);
	correctedSumAB->setInputDataSourcesImplementation(correctedSpectrumDataSources_);
	correctedSpectrumDataSources_ << correctedSumAB;
}

XRFDetector::XRFDetector(QString name, AMControl *status, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControl *deadTime, AMControl *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(1);

	wasConnected_ = false;
	detectorConnected_ = false;
	timer_.setInterval(6000);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));

	statusControl_ = status;
	refreshRateControl_ = refreshRate;
	peakingTimeControl_ = peakingTime;
	maximumEnergyControl_ = maximumEnergy;
	integrationTimeControl_ = integrationTime;
	liveTimeControl_ = liveTime;
	elapsedTimeControl_ = elapsedTime;
	startControl_ = start;
	stopControl_ = stop;

	deadTimeControl_ = new AMControlSet(this);
	deadTimeControl_->addControl(deadTime);
	spectraControl_ = new AMControlSet(this);
	spectraControl_->addControl(spectra);

	readingControls_ = new AMControlSet(this);
	settingsControls_ = new AMControlSet(this);

	readingControls_->addControl(status);
	readingControls_->addControl(elapsedTime);
	for (int i = 0; i < deadTimeControl_->count(); i++)
		readingControls_->addControl(deadTimeControl_->at(i));
	for (int i = 0; i < spectraControl_->count(); i++)
		readingControls_->addControl(spectraControl_->at(i));

	settingsControls_->addControl(refreshRateControl_);
	settingsControls_->addControl(peakingTimeControl_);
	settingsControls_->addControl(maximumEnergyControl_);
	settingsControls_->addControl(integrationTimeControl_);
	settingsControls_->addControl(liveTimeControl_);
	settingsControls_->addControl(startControl_);
	settingsControls_->addControl(stopControl_);

	connect(readingControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));

	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SIGNAL(maximumEnergyChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(peakingTimeChanged(double)));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(integrationTimeChanged(double)));
	connect(elapsedTimeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(elapsedTimeChanged(double)));
	connect(refreshRateControl_, SIGNAL(valueChanged(double)), this, SIGNAL(refreshRateChanged(double)));
	connect(deadTimeControl_, SIGNAL(controlSetValuesChanged()), this, SIGNAL(deadTimeChanged()));
	connect(spectraControl_, SIGNAL(controlSetValuesChanged()), this, SIGNAL(spectraChanged()));

	connect(statusControl_, SIGNAL(valueChanged(double)), this, SIGNAL(statusChanged()));
	connect(integrationTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setIntegrationTime(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(setPeakingTime(double)));
	connect(maximumEnergyControl_, SIGNAL(valueChanged(double)), this, SLOT(setMaximumEnergy(double)));
	connect(spectraControl_, SIGNAL(controlSetValuesChanged()), this, SLOT(setChannelSize()));

	AMReadOnlyPVControl *spectrum = qobject_cast<AMReadOnlyPVControl *>(spectraControl_->at(0));
	AMReadOnlyPVControl *deadTimePV = qobject_cast<AMReadOnlyPVControl *>(deadTime);

	if (spectrum != 0 && deadTimePV != 0){

		spectrumDataSources_ << new AM1DProcessVariableDataSource(spectrum->readPV(), "Raw Spectrum", this);
		deadTimeDataSources_ << new AM0DProcessVariableDataSource(deadTimePV->readPV(), "Dead time", this);
		AMDeadTimeAB *corrected = new AMDeadTimeAB("Corrected Sum", this);
		QList<AMDataSource *> corrList;
		corrList << (AMDataSource *)spectrumDataSources_.first() << (AMDataSource *)deadTimeDataSources_.first();
		corrected->setInputDataSourcesImplementation(corrList);
		correctedSpectrumDataSources_ << corrected;
	}
}

XRFDetector::~XRFDetector()
{
	if (elements() == 1){

		delete deadTimeControl_;
		delete spectraControl_;
	}

	delete readingControls_;
	delete settingsControls_;

	while(!roiList_.isEmpty()){

		delete roiList_.takeLast();
	}
}

void XRFDetector::setRoiList(QList<AMROI *> list)
{
	roiList_ << list;

	for (int i = 0; i < roiList_.size(); i++){

		connect(roiList_.at(i), SIGNAL(roiConnected(bool)), this, SLOT(detectorConnected()));
		connect(roiList_.at(i), SIGNAL(roiHasValues()), this, SLOT(allRoisHaveValues()));
	}
}

double XRFDetector::deadTime() const
{
	// For the single element, return the value.  For multi-element detectors, return the worst.
	if (elements_ == 1)
		return deadTimeControl()->at(0)->value();

	else {

		double dt = 0;

		for (int i = 0; i < elements_; i++) {

			if (dt < deadTimeControl()->at(i)->value())
				dt = deadTimeControl()->at(i)->value();
		}

		return dt;
	}
}

void XRFDetector::setChannelSize()
{
	AMReadOnlyPVControl *spectra = qobject_cast<AMReadOnlyPVControl *>(spectraControl_->at(0));
	if (spectra){

		setSize(AMnDIndex(spectra->readPV()->count()));
		for (int i = 0; i < spectrumDataSources_.size(); i++)
			spectrumDataSources_.at(i)->setScale(scale());

		// Don't need to come here again because the size of the detector is static.
		disconnect(spectraControl_, SIGNAL(controlSetValuesChanged()), this, SLOT(setChannelSize()));
	}
}

bool XRFDetector::setFromInfo(const AMDetectorInfo *info)
{
	const XRFDetectorInfo *detectorInfo = qobject_cast<const XRFDetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	refreshRateControl()->move(detectorInfo->refreshRate());
	peakingTimeControl()->move(detectorInfo->peakingTime());
	maximumEnergyControl()->move(detectorInfo->maximumEnergy());
	integrationTimeControl()->move(detectorInfo->integrationTime());
	for (int i = 0; i < detectorInfo->roiInfoList()->count(); i++)
		roiList().at(i)->setRegion(detectorInfo->roiInfoList()->at(i));

	return true;
}

void XRFDetector::fromXRFInfo(const XRFDetectorInfo &info)
{
	refreshRateControl()->move(info.refreshRate());
	peakingTimeControl()->move(info.peakingTime());
	maximumEnergyControl()->move(info.maximumEnergy());
	integrationTimeControl()->move(info.integrationTime());
	for (int i = 0; i < info.roiInfoList()->count(); i++)
		roiList().at(i)->setRegion(info.roiInfoList()->at(i));
}

void XRFDetector::setRefreshRateControl(XRFDetectorInfo::MCAUpdateRate rate)
{
	setRefreshRate(rate);

	switch(rate){
	case XRFDetectorInfo::Passive:
		refreshRateControl()->move(0);
		break;
	case XRFDetectorInfo::Slow:
		refreshRateControl()->move(6);
		break;
	case XRFDetectorInfo::Fast:
		refreshRateControl()->move(8);
		break;
	}
}

void XRFDetector::detectorConnected()
{
	wasConnected_ = detectorConnected_;

	bool connected = true;

	for (int i = 0; i < roiList().size(); i++)
		connected = connected && roiList().at(i)->isConnected();

	connected = connected && readingControls_->isConnected() && settingsControls_->isConnected();

	if (detectorConnected_ != connected){

		detectorConnected_ = connected;
		emit detectorConnected(detectorConnected_);
	}
}

void XRFDetector::allRoisHaveValues()
{
	bool hasValues = true;

	for (int i = 0; i < roiList().size(); i++)
		hasValues = hasValues && roiList().at(i)->hasValues();

	if (hasValues)
		emit roisHaveValues();

	if (hasValues){

		for (int i = 0; i < roiList_.size(); i++)
			connect(roiList_.at(i), SIGNAL(roiUpdate(AMROI*)), this, SIGNAL(roiUpdate(AMROI*)));

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
	roiList().at(roiInfoList()->count())->fromInfo(roi);
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

		if (i+1 == roiInfoList()->count()){
			roiList().at(i)->setRegion("", -1, -1);
		}
		else
			roiList().at(i)->fromInfo(roiInfoList()->at(i+1));
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
		roiList().at(i)->fromInfo(roiInfoList()->at(i));
}

void XRFDetector::clearRegionsOfInterest()
{
	for (int i = 0; i < roiList().count(); i++)
		roiList().at(i)->setRegion("", -1, -1);

	roiInfoList()->clear();
	setROIList(*roiInfoList());
}

void XRFDetector::enableElement(int id)
{
	activeElements_[id] = true;

	QList<AMDataSource *> newSum;
	for (int i = 0; i < elements(); i++)
		if (activeElements_.at(i))
			newSum << correctedSpectrumDataSources_.at(i);

	((AM1DSummingAB *)correctedSumDataSource())->setInputDataSourcesImplementation(newSum);
}

void XRFDetector::disableElement(int id)
{
	activeElements_[id] = false;

	QList<AMDataSource *> newSum;
	for (int i = 0; i < elements(); i++)
		if (activeElements_.at(i))
			newSum << correctedSpectrumDataSources_.at(i);

	((AM1DSummingAB *)correctedSumDataSource())->setInputDataSourcesImplementation(newSum);
}

QVector<int> XRFDetector::spectraValues(int index)
{
	if (index < elements() && index >= 0){

		AMReadOnlyPVControl *temp = qobject_cast<AMReadOnlyPVControl *>(spectraControl_->at(index));
		return temp->readPV()->lastIntegerValues();
	}

	return QVector<int>();
}

double XRFDetector::deadTimeAt(int index)
{
	if (index < elements() && index >= 0){

		AMReadOnlyPVControl *temp = qobject_cast<AMReadOnlyPVControl *>(deadTimeControl_->at(index));
		return temp->readPV()->lastValue();
	}

	return -1;
}

void XRFDetector::setTime(double time)
{
	setIntegrationTime(time);
	integrationTimeControl()->move(time);
	liveTimeControl()->move(0.0);
}

void XRFDetector::setMaximumEnergyControl(double energy)
{
	setMaximumEnergy(energy);
	maximumEnergyControl()->move(energy);
}

void XRFDetector::setPeakingTimeControl(double time)
{
	setPeakingTime(time);
	peakingTimeControl()->move(time);
}
