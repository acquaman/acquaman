#include "XRFDetector.h"

XRFDetectorDataSource::XRFDetectorDataSource(AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
}

void XRFDetectorDataSource::onDataChanged()
{
	emitDataChanged();
}

XRFDetector::XRFDetector(QString name, int elements, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControlSet *deadTime, AMControlSet *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(elements);
	setActiveElements(elements);
	usingSingleElement_ = false;

	wasConnected_ = false;
	detectorConnected_ = false;

	refreshRateControl_ = refreshRate;
	peakingTimeControl_ = peakingTime;
	maximumEnergyControl_ = maximumEnergy;
	integrationTimeControl_ = integrationTime;
	liveTimeControl_ = liveTime;
	elapsedTimeControl_ = elapsedTime;
	deadTimeControl_ = deadTime;
	spectraControl_ = spectra;
	startControl_ = start;
	stopControl_ = stop;

	readingControls_ = new AMControlSet(this);
	settingsControls_ = new AMControlSet(this);

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
	connect(readingControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SIGNAL(readingsChanged(AMControlInfoList)));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));
	connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SIGNAL(settingsChanged(AMControlInfoList)));

	for (int i = 0; i < roiList().size(); i++)
		connect(roiList().at(i), SIGNAL(roiConnected(bool)), this, SLOT(detectorConnected(bool)));
}

XRFDetector::XRFDetector(QString name, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControl *deadTime, AMControl *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(1);
	setActiveElements(1);
	usingSingleElement_ = true;

	wasConnected_ = false;
	detectorConnected_ = false;

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
	connect(readingControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SIGNAL(readingsChanged(AMControlInfoList)));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected()));
	connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SIGNAL(settingsChanged(AMControlInfoList)));

	for (int i = 0; i < roiList().size(); i++)
		connect(roiList().at(i), SIGNAL(roiConnected(bool)), this, SLOT(detectorConnected(bool)));
}

XRFDetector::~XRFDetector()
{
	if (usingSingleElement_){

		delete deadTimeControl_;
		delete spectraControl_;
	}

	delete readingControls_;
	delete settingsControls_;
}

double XRFDetector::deadTime() const
{
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

	detectorConnected_ = true;

	//for (int i = 0; i < roiList().size(); i++)
		//detectorConnected_ = detectorConnected_ && roiList().at(i)->isConnected();

	detectorConnected_ = detectorConnected_ && readingControls_->isConnected() && settingsControls_->isConnected();

	emit detectorConnected(detectorConnected_);
}

bool XRFDetector::addRegionOfInterest(AMROIInfo roi)
{
	// No more ROIs.
	if (roiInfoList()->count() == roiList().size())
		return false;

	// Appending to the list means that the old size of the Info list is where the new values should be set in the ROI list.
	roiList().at(roiInfoList()->count())->fromInfo(roi);
	roiInfoList()->append(roi);

	return true;
}

bool XRFDetector::removeRegionOfInterest(QString name)
{
	int indexOfRemoved = roiInfoList()->indexOf(name);

	if (indexOfRemoved == -1)
		return false;

	for (int i = indexOfRemoved; i < roiInfoList()->count(); i++){

		if (i+1 == roiInfoList()->count())
			roiList().at(i)->fromInfo(AMROIInfo(""));

		roiList().at(i)->fromInfo(roiInfoList()->at(i+1));
	}

	roiInfoList()->remove(indexOfRemoved);

	return true;
}

void XRFDetector::sort()
{
	roiInfoList()->sort();

	for (int i = 0; i < roiInfoList()->count(); i++)
		roiList().at(i)->fromInfo(roiInfoList()->at(i));
}

void XRFDetector::enableElement(int id)
{
	/// \todo Need to implement enabling waveforms.
}

void XRFDetector::disableElement(int id)
{
	/// \todo Need to implement disabling waveforms.
}

const double *XRFDetector::spectraAt(int index)
{
	if (index < elements() && index >= 0){

		AMReadOnlyPVControl *temp = qobject_cast<AMReadOnlyPVControl *>(spectraControl_->at(index));
		QVector<double> array(temp->readPV()->lastFloatingPointValues());
		return array.constData();
	}

	return 0;
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
