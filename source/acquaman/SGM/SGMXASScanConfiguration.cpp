#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
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
    AMXASRegion *xasr = new AMXASRegion(SGMBeamline::sgm()->energy(), this);
    xasr->setStart(start);
    xasr->setDelta(delta);
    xasr->setEnd(end);
    return addRegion(index, xasr);
}
