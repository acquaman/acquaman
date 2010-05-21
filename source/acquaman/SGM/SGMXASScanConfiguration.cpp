#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();
	XASDetectors_ = SGMBeamline::sgm()->XASDetectors();
	qDebug() << "Gap: " << exitSlitGap_ << " grating " << grating_ << " utrack " << undulatorTracking_ << " mtrack " << monoTracking_ << " xtrack " << exitSlitTracking_;
	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit undulatorTrackingChanged(undulatorTracking_);
	emit monoTrackingChanged(monoTracking_);
	emit exitSlitTrackingChanged(exitSlitTracking_);
}

QStringList SGMXASScanConfiguration::usingDetectors() const{
	QStringList usingDetectors;
	if(usingTEY_)
		usingDetectors << "tey";
	if(usingTFY_)
		usingDetectors << "tfy";
	if(usingPGT_)
		usingDetectors << "pgt";
	return usingDetectors;
}
