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

	xasDetectors_ = SGMBeamline::sgm()->XASDetectorsNew();

	allDetectors_ = new AMDetectorSet(this);
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectorsNew()->count(); x++)
		allDetectors_->addDetector(SGMBeamline::sgm()->feedbackDetectorsNew()->detectorAt(x), true);
	for(int x = 0; x < xasDetectors_->count(); x++)
		allDetectors_->addDetector(xasDetectors_->detectorAt(x), xasDetectors_->isDefaultAt(x));
	xasDetectorsCfg_ = xasDetectors_->toInfoSet();

	feedbackDetectorsOld_ = SGMBeamline::sgm()->feedbackDetectors();
	XASDetectorsOld_ = SGMBeamline::sgm()->XASDetectors();

	// default channels removed. Need to come up with new replacement system to create default analysis blocks instead.

	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit trackingGroupChanged(trackingGroup_);
}

AMDetectorInfoSet SGMXASScanConfiguration::allDetectorConfigurations() const{
	AMDetectorInfoSet allConfigurations;
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectorsNew()->count(); x++)
		allConfigurations.addDetectorInfo(SGMBeamline::sgm()->feedbackDetectorsNew()->detectorAt(x)->toInfo(), true);
	for(int x = 0; x < xasDetectorsCfg_.count(); x++)
		allConfigurations.addDetectorInfo(xasDetectorsCfg_.detectorInfoAt(x), xasDetectorsCfg_.isActiveAt(x));
	return allConfigurations;
}

QList<AMDetectorInfo*> SGMXASScanConfiguration::usingDetectors() const{
	QList<AMDetectorInfo*> usingDetectors;
	usingDetectors << feedbackDetectorsOld_->detectorByName("I0");
	usingDetectors << feedbackDetectorsOld_->detectorByName("eVFbk");
	if(usingTEY_)
		usingDetectors << XASDetectorsOld_->detectorByName("tey");
	if(usingTFY_)
		usingDetectors << XASDetectorsOld_->detectorByName("tfy");
	if(usingPGT_)
		usingDetectors << XASDetectorsOld_->detectorByName("pgt");
	return usingDetectors;
}

AMScanConfiguration* SGMXASScanConfiguration::createCopy() const{
	return new SGMXASScanConfiguration(*this);
}

#include "SGMXASDacqScanController.h"

AMScanController* SGMXASScanConfiguration::createController(){
	return new SGMXASDacqScanController(this);
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

bool SGMXASScanConfiguration::setTrackingGroup(AMControlInfoList trackingList){
	bool rVal = SGMScanConfiguration::setTrackingGroup(trackingList);
	emit trackingGroupChanged(trackingList);
	return rVal;
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

bool SGMXASScanConfiguration::setDetectorConfigurations(const AMDetectorInfoSet &xasDetectorsCfg){
	xasDetectorsCfg_ = xasDetectorsCfg;
}
