#include "VESPERSEnergyScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSEnergyDacqScanController.h"
#include "ui/VESPERS/VESPERSEnergyScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(QObject *parent)
	: AMXASScanConfiguration(parent)
{
	setName("Energy Scan");
	setUserScanName("Energy Scan");
	regions_->setSensibleRange(10000, 20000);
	xasRegions()->setEnergyControl(VESPERSBeamline::vespers()->energy());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");

	ccdDetector_ = Roper;
	ccdFileName_ = "";
	goToPosition_ = false;
	position_ = qMakePair(0.0, 0.0);
	totalTime_ = 0;
	timeOffset_ = 6;
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
}

VESPERSEnergyScanConfiguration::VESPERSEnergyScanConfiguration(const VESPERSEnergyScanConfiguration &original)
	: AMXASScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	regions_->setSensibleRange(original.regions()->sensibleStart(), original.regions()->sensibleEnd());
	xasRegions()->setEnergyControl(VESPERSBeamline::vespers()->energy());
	regions_->setDefaultTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for (int i = 0; i < original.regionCount(); i++)
		regions_->addRegion(i, original.regionStart(i), original.regionDelta(i), original.regionEnd(i), original.regionTime(i));

	ccdDetector_ = original.ccdDetector();
	ccdFileName_ = original.ccdFileName();
	goToPosition_ = original.goToPosition();
	position_ = original.position();
	timeOffset_ = 6;
	totalTime_ = 0;
	computeTotalTime();
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERSEnergyScanConfiguration::createCopy() const
{
	return new VESPERSEnergyScanConfiguration(*this);
}

AMScanController *VESPERSEnergyScanConfiguration::createController()
{
	return new VESPERSEnergyDacqScanController(this);
}

AMScanConfigurationView *VESPERSEnergyScanConfiguration::createView()
{
	return new VESPERSEnergyScanConfigurationView(this);
}

QString VESPERSEnergyScanConfiguration::detailedDescription() const
{
	return QString("VESPERS Energy Scan");
}

QString VESPERSEnergyScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(QString("CCD detector used: %1\n").arg(ccdDetector() == Roper ? "Roper" : "Mar"));
	header.append(QString("Automatically moved to a specific location (used when setting up the workflow)?\t%1").arg(goToPosition() ? "Yes\n" : "No\n\n"));

	if (goToPosition()){

		header.append(QString("Horizontal Position:\t%1 mm\n").arg(x()));
		header.append(QString("Vertical Position:\t%1 mm\n\n").arg(y()));
	}

	header.append("\n");
	header.append("Regions Scanned\n");

	for (int i = 0; i < regionCount(); i++){

		header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
						  .arg(regionStart(i))
						  .arg(regionDelta(i))
						  .arg(regionEnd(i))
						  .arg(regionTime(i)));
	}

	header.append(QString("\nFilename for XRD images:\t%1\n").arg(ccdFileName()));

	return header;
}

void VESPERSEnergyScanConfiguration::computeTotalTime()
{
	double time = 0;

	for (int i = 0; i < regions_->count(); i++)
		time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_); // Seems to take about 0.7 seconds for extra beamline stuff to happen.

	totalTime_ = time + 9; // There is a 9 second miscellaneous startup delay.
	emit totalTimeChanged(totalTime_);
}

void VESPERSEnergyScanConfiguration::setTimeOffset(double offset)
{
	if (timeOffset_ != offset){

		timeOffset_ = offset;
		computeTotalTime();
	}
}

void VESPERSEnergyScanConfiguration::setCcdDetector(CCDDetector detector)
{
	if (ccdDetector_ != detector){

		ccdDetector_ = detector;
		emit ccdDetectorChanged(ccdDetector_);
		emit ccdDetectorChanged(int(ccdDetector_));
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::setGoToPosition(bool state)
{
	if (goToPosition_ != state){

		goToPosition_ = state;
		emit gotoPositionChanged(goToPosition_);
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::setPosition(QPair<double, double> pos)
{
	setX(pos.first);
	setY(pos.second);
}

void VESPERSEnergyScanConfiguration::setX(double xPos)
{
	if (position_.first != xPos){

		position_.first = xPos;
		emit xPositionChanged(xPos);
		setModified(true);
	}
}

void VESPERSEnergyScanConfiguration::setY(double yPos)
{
	if (position_.second != yPos){

		position_.second = yPos;
		emit yPositionChanged(yPos);
		setModified(true);
	}
}