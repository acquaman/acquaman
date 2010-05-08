#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();
	qDebug() << "Gap: " << exitSlitGap_ << " grating " << grating_ << " utrack " << undulatorTracking_ << " mtrack " << monoTracking_ << " xtrack " << exitSlitTracking_;
	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit undulatorTrackingChanged(undulatorTracking_);
	emit monoTrackingChanged(monoTracking_);
	emit exitSlitTrackingChanged(exitSlitTracking_);
}

bool SGMXASScanConfiguration::addRegion(size_t index, double start, double delta, double end)
{
	qDebug() << "Calling add region in scan cfg";
	return regions_->addRegion(index, start, delta, end);
	/*
	AMXASRegion *xasr = new AMXASRegion(SGMBeamline::sgm()->energy(), this);
	if(!xasr->setStart(start) || !xasr->setDelta(delta) || !xasr->setEnd(end))
		return false;
	return addRegion(index, xasr);
	*/
}
