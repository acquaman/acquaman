/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();
	feedbackDetectors_ = SGMBeamline::sgm()->feedbackDetectors();
	XASDetectors_ = SGMBeamline::sgm()->XASDetectors();

	// default channels removed. Need to come up with new replacement system to create default analysis blocks instead.

	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit undulatorTrackingChanged(undulatorTracking_);
	emit monoTrackingChanged(monoTracking_);
	emit exitSlitTrackingChanged(exitSlitTracking_);
}

QList<AMDetectorInfo*> SGMXASScanConfiguration::usingDetectors() const{
	QList<AMDetectorInfo*> usingDetectors;
	usingDetectors << feedbackDetectors_->detectorByName("I0");
	usingDetectors << feedbackDetectors_->detectorByName("eVFbk");
	if(usingTEY_)
		usingDetectors << XASDetectors_->detectorByName("tey");
	if(usingTFY_)
		usingDetectors << XASDetectors_->detectorByName("tfy");
	if(usingPGT_)
		usingDetectors << XASDetectors_->detectorByName("pgt");
	return usingDetectors;
}

AMScanConfiguration* SGMXASScanConfiguration::createCopy() const{

}

AMScanController* SGMXASScanConfiguration::createController(){

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
