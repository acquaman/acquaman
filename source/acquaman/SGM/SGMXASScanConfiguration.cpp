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
	tmpCh.second = "2000*tey/I0";
	defaultChannels_.append(tmpCh);
	tmpCh.first = "NormTFY";
	tmpCh.second = "-2000*tfy/I0";
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

QList<AMDetectorInfo*> SGMXASScanConfiguration::usingDetectors() const{
	QList<AMDetectorInfo*> usingDetectors;
	if(usingTEY_)
		usingDetectors << XASDetectors_->detectorByName("tey");
	if(usingTFY_)
		usingDetectors << XASDetectors_->detectorByName("tfy");
	if(usingPGT_)
		usingDetectors << XASDetectors_->detectorByName("pgt");
	return usingDetectors;
}

bool SGMXASScanConfiguration::setExitSlitGap(double exitSlitGap) {
	bool rVal = SGMScanConfiguration::setExitSlitGap(exitSlitGap);
	emit exitSlitGapChanged(exitSlitGap);
	return rVal;
}

bool SGMXASScanConfiguration::setGrating(SGMBeamline::sgmGrating grating) {
	bool rVal = SGMScanConfiguration::setGrating(grating);
	emit gratingChanged(grating);
	return rVal;
}

bool SGMXASScanConfiguration::setGrating(int grating) {
	return setGrating( (SGMBeamline::sgmGrating)grating );
}

bool SGMXASScanConfiguration::setHarmonic(SGMBeamline::sgmHarmonic harmonic) {
	bool rVal = SGMScanConfiguration::setHarmonic(harmonic);
	emit harmonicChanged(harmonic);
	return rVal;
}

bool SGMXASScanConfiguration::setHarmonic(int harmonic) {
	return setHarmonic( (SGMBeamline::sgmHarmonic)harmonic );
}

bool SGMXASScanConfiguration::setUndulatorTracking(bool track){
	bool rVal = SGMScanConfiguration::setUndulatorTracking(track);
	emit undulatorTrackingChanged(track);
	return rVal;
}

bool SGMXASScanConfiguration::setUndulatorTracking(int track){
	return setUndulatorTracking( (bool)track);
}

bool SGMXASScanConfiguration::setMonoTracking(bool track){
	bool rVal = SGMScanConfiguration::setMonoTracking(track);
	emit monoTrackingChanged(track);
	return rVal;
}

bool SGMXASScanConfiguration::setMonoTracking(int track){
	return setMonoTracking( (bool)track );
}

bool SGMXASScanConfiguration::setExitSlitTracking(bool track){
	bool rVal = SGMScanConfiguration::setExitSlitTracking(track);
	emit exitSlitTrackingChanged(track); return rVal;
}

bool SGMXASScanConfiguration::setExitSlitTracking(int track){
	return setExitSlitTracking( (bool)track );
}

bool SGMXASScanConfiguration::setUsingTEY(bool active) {
	bool rVal = SGMScanConfiguration::setUsingTEY(active);
	emit usingTEYChanged(active);
	return rVal;
}

bool SGMXASScanConfiguration::setUsingTEY(int checkedState) {
	return setUsingTEY( (bool)checkedState);
}

bool SGMXASScanConfiguration::setUsingTFY(bool active) {
	bool rVal = SGMScanConfiguration::setUsingTFY(active);
	emit usingTFYChanged(active);
	return rVal;
}

bool SGMXASScanConfiguration::setUsingTFY(int checkedState) {
	return setUsingTFY( (bool)checkedState);
}

bool SGMXASScanConfiguration::setUsingPGT(bool active) {
	bool rVal = SGMScanConfiguration::setUsingPGT(active);
	emit usingPGTChanged(active);
	return rVal;
}

bool SGMXASScanConfiguration::setUsingPGT(int checkedState) {
	return setUsingPGT( (bool)checkedState);
}
