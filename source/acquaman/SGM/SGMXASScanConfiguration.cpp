#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();
	XASDetectors_ = SGMBeamline::sgm()->XASDetectors();
	QPair<QString, QString> tmpCh;
	tmpCh.first = "eV";
	tmpCh.second = "eV";
	defaultChannels_.append(tmpCh);
	tmpCh.first = "NormTEY";
	tmpCh.second = "tey/I0";
	defaultChannels_.append(tmpCh);
	tmpCh.first = "NormTFY";
	tmpCh.second = "tfy/I0";
	defaultChannels_.append(tmpCh);
	tmpCh.first = "PGT@130";
	tmpCh.second = "pgt.pgtCounts[130]";
	defaultChannels_.append(tmpCh);
	tmpCh.first = "Nonsense";
	tmpCh.second = "tf/I0+bacon";
	defaultChannels_.append(tmpCh);
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
