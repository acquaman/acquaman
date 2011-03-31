#include "XRFDetector.h"

XRFDetector::XRFDetector(QString name, int elements, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControlSet *deadTime, AMControlSet *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(elements);
	setActiveElements(elements);

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

	connect(readingControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected(bool)));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected(bool)));

	for (int i = 0; i < roiList().size(); i++)
		connect(roiList().at(i), SIGNAL(roiConnected(bool)), this, SLOT(detectorConnected(bool)));
}

XRFDetector::XRFDetector(QString name, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControl *deadTime, AMControl *spectra, QObject *parent)
	: XRFDetectorInfo(name, name, parent), AMDetector(name)
{
	setElements(1);
	setActiveElements(1);

	refreshRateControl_ = refreshRate;
	peakingTimeControl_ = peakingTime;
	maximumEnergyControl_ = maximumEnergy;
	integrationTimeControl_ = integrationTime;
	liveTimeControl_ = liveTime;
	elapsedTimeControl_ = elapsedTime;
	deadTimeControl_->addControl(deadTime);
	spectraControl_->addControl(spectra);
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

	connect(readingControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected(bool)));
	connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(detectorConnected(bool)));

	for (int i = 0; i < roiList().size(); i++)
		connect(roiList().at(i), SIGNAL(roiConnected(bool)), this, SLOT(detectorConnected(bool)));
}

XRFDetector::~XRFDetector()
{
	delete readingControls_;
	delete settingsControls_;
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
	bool detectorConnected_ = true;

	for (int i = 0; i < roiList().size(); i++)
		detectorConnected_ &= roiList().at(i)->isConnected();

	detectorConnected_ = detectorConnected_ && readingControls_->isConnected() && settingsControls_->isConnected();

	emit connected(detectorConnected_);
}
