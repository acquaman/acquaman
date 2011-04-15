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

	xasDetectors_ = SGMBeamline::sgm()->XASDetectors();

	allDetectors_ = new AMDetectorSet(this);
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectors()->count(); x++)
		allDetectors_->addDetector(SGMBeamline::sgm()->feedbackDetectors()->detectorAt(x), true);
	for(int x = 0; x < xasDetectors_->count(); x++)
		allDetectors_->addDetector(xasDetectors_->detectorAt(x), xasDetectors_->isDefaultAt(x));
	xasDetectorsCfg_ = xasDetectors_->toInfoSet();

	// default channels removed. Need to come up with new replacement system to create default analysis blocks instead.

	emit exitSlitGapChanged(exitSlitGap_);
	emit gratingChanged(grating_);
	emit trackingGroupChanged(trackingGroup_);
}

SGMXASScanConfiguration::SGMXASScanConfiguration(const SGMXASScanConfiguration &original){
	regions_->setEnergyControl(SGMBeamline::sgm()->energy());
	for(int x = 0; x < original.regionCount(); x++)
		regions_->addRegion(x, original.regionStart(x), original.regionDelta(x), original.regionEnd(x));

	fluxResolutionSet_ = SGMBeamline::sgm()->fluxResolutionSet();
	trackingSet_ = SGMBeamline::sgm()->trackingSet();

	xasDetectors_ = SGMBeamline::sgm()->XASDetectors();

	allDetectors_ = new AMDetectorSet(this);
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectors()->count(); x++)
		allDetectors_->addDetector(SGMBeamline::sgm()->feedbackDetectors()->detectorAt(x), true);
	for(int x = 0; x < xasDetectors_->count(); x++)
		allDetectors_->addDetector(xasDetectors_->detectorAt(x), xasDetectors_->isDefaultAt(x));

	setTrackingGroup(original.trackingGroup());
	setFluxResolutionGroup(original.fluxResolutionGroup());
	setDetectorConfigurations(original.detectorChoiceConfigurations());
}

AMDetectorInfoSet SGMXASScanConfiguration::allDetectorConfigurations() const{
	AMDetectorInfoSet allConfigurations;
	for(int x = 0; x < SGMBeamline::sgm()->feedbackDetectors()->count(); x++)
		allConfigurations.addDetectorInfo(SGMBeamline::sgm()->feedbackDetectors()->detectorAt(x)->toInfo(), true);
	for(int x = 0; x < xasDetectorsCfg_.count(); x++)
		allConfigurations.addDetectorInfo(xasDetectorsCfg_.detectorInfoAt(x), xasDetectorsCfg_.isActiveAt(x));
	return allConfigurations;
}

AMScanConfiguration* SGMXASScanConfiguration::createCopy() const{
	return new SGMXASScanConfiguration(*this);
}

#include "SGMXASDacqScanController.h"

AMScanController* SGMXASScanConfiguration::createController(){
	return new SGMXASDacqScanController(this);
}

#include "ui/SGMXASScanConfigurationView.h"

AMScanConfigurationView* SGMXASScanConfiguration::createView(){
	return new SGMXASScanConfigurationView(this);
}

QString SGMXASScanConfiguration::detailedDescription() const{
	double exitSlit;
	double grating;
	double harmonic;
	for(int x = 0; x < fluxResolutionGroup_.count(); x++){
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->exitSlitGap()->name())
			exitSlit = fluxResolutionGroup_.at(x).value();
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->grating()->name())
			grating = fluxResolutionGroup_.at(x).value();
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->harmonic()->name())
			harmonic = fluxResolutionGroup_.at(x).value();
	}
	return QString("XAS Scan from %1 to %2\nExit Slit: %3\nGrating: %4\nHarmonic: %5").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(exitSlit, 0, 'f', 1).arg(SGMBeamline::sgm()->sgmGratingDescription(SGMBeamline::sgmGrating(grating))).arg(SGMBeamline::sgm()->sgmHarmonicDescription(SGMBeamline::sgmHarmonic(harmonic)));
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

bool SGMXASScanConfiguration::setFluxResolutionGroup(AMControlInfoList fluxResolutionList){
	bool rVal = SGMScanConfiguration::setFluxResolutionGroup(fluxResolutionList);
	emit fluxResolutionGroupChanged(fluxResolutionList);
	return rVal;
}

bool SGMXASScanConfiguration::setDetectorConfigurations(const AMDetectorInfoSet &xasDetectorsCfg){
	xasDetectorsCfg_ = xasDetectorsCfg;
}
