#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();
	XASDetectors_ = SGMBeamline::sgm()->XASDetectors();
	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit undulatorTrackingChanged(undulatorTracking_);
	emit monoTrackingChanged(monoTracking_);
	emit exitSlitTrackingChanged(exitSlitTracking_);
}

QList<AMAbstractDetector*> SGMXASScanConfiguration::usingDetectors() const{
	QList<AMAbstractDetector*> usingDetectors;
	if(usingTEY_)
		usingDetectors << XASDetectors_->detectorByName("tey");
	if(usingTFY_)
		usingDetectors << XASDetectors_->detectorByName("tfy");
	if(usingPGT_)
		usingDetectors << XASDetectors_->detectorByName("pgt");
	return usingDetectors;
}
