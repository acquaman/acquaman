/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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

 SGMXASScanConfiguration::~SGMXASScanConfiguration(){}
SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
	if(SGMBeamline::sgm()->isConnected()){
		xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());
		regions_->setDefaultTimeControl(SGMBeamline::sgm()->masterDwell());
		connect(SGMBeamline::sgm()->exitSlitGap(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
		connect(SGMBeamline::sgm()->grating(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
		connect(SGMBeamline::sgm()->harmonic(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
		connect(SGMBeamline::sgm()->undulatorTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
		connect(SGMBeamline::sgm()->monoTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
		connect(SGMBeamline::sgm()->exitSlitTracking(), SIGNAL(valueChanged(double)), this,SLOT(checkIfMatchesBeamline()));

	}

	regions_->setSensibleRange(200, 2000);
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
	connect(this, SIGNAL(configurationChanged()), this, SLOT(checkIfMatchesBeamline()));

}


SGMXASScanConfiguration::SGMXASScanConfiguration(const SGMXASScanConfiguration &original) : AMXASScanConfiguration(original) , SGMScanConfiguration()
{
	if(SGMBeamline::sgm()->isConnected()){
		xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());
		regions_->setDefaultTimeControl(SGMBeamline::sgm()->scalerIntegrationTime());

	}

	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for(int x = 0; x < original.regionCount(); x++)
		regions_->addRegion(x, original.regionStart(x), original.regionDelta(x), original.regionEnd(x), original.regionTime(x));

	setTrackingGroup(original.trackingGroup());
	setFluxResolutionGroup(original.fluxResolutionGroup());
	setDetectorConfigurations(original.detectorChoiceConfigurations());
	connect(this, SIGNAL(configurationChanged()), this, SLOT(checkIfMatchesBeamline()));
}

const QMetaObject* SGMXASScanConfiguration::getMetaObject(){
	return metaObject();
}

bool SGMXASScanConfiguration::canEnumConvert(const QString &enumName) const{
	if(enumName == "grating" || enumName == "harmonic")
		return true;
	return false;
}

QString SGMXASScanConfiguration::enumConvert(const QString &enumName, int enumValue) const{
	if(enumName == "grating"){
		switch(enumValue){
		case 0:
			return "Low";
			break;
		case 1:
			return "Medium";
			break;
		case 2:
			return "High";
			break;
		}
	}
	else if(enumName == "harmonic"){
		switch(enumValue){
		case 0:
			return "First";
			break;
		case 1:
			return "Third";
			break;
		}
	}
	return "[??]";
}

AMScanConfiguration* SGMXASScanConfiguration::createCopy() const{
	return new SGMXASScanConfiguration(*this);
}

AMScanController* SGMXASScanConfiguration::createController(){
	return 0; //NULL
}

#include "ui/SGM/SGMXASScanConfigurationView.h"

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
	(void)grating; /* Added to supress erroneous compiler warnings, see Issue734 */
	(void)harmonic; /* Added to supress erroneous compiler warnings, see Issue734 */
	return QString("XAS Scan from %1 to %2\nExit Slit: %3\nGrating: %4\nHarmonic: %5").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(exitSlit, 0, 'f', 1).arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(SGMBeamlineInfo::sgmGrating(grating))).arg(SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription(SGMBeamlineInfo::sgmHarmonic(harmonic)));
}

QString SGMXASScanConfiguration::dbLoadWarnings() const{
	return xasDetectorsCfg_.dbLoadWarnings();
}

bool SGMXASScanConfiguration::setTrackingGroup(AMControlInfoList trackingList){
	bool oldUndulator = undulatorTracking_;
	bool oldMono = monoTracking_;
	bool oldExitSlit = exitSlitTracking_;

	bool rVal = SGMScanConfiguration::setTrackingGroup(trackingList);
	if(rVal){
		emit trackingGroupChanged(trackingList);
		if(oldUndulator != undulatorTracking_)
			emit undulatorTrackingChanged(undulatorTracking_);
		if(oldMono != monoTracking_)
			emit monoTrackingChanged(monoTracking_);
		if(oldExitSlit != exitSlitTracking_)
			emit exitSlitTrackingChanged(exitSlitTracking_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setFluxResolutionGroup(AMControlInfoList fluxResolutionList){
	bool rVal = SGMScanConfiguration::setFluxResolutionGroup(fluxResolutionList);
	if(rVal){
		emit fluxResolutionGroupChanged(fluxResolutionList);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setUndulatorTracking(bool undulatorTracking){
	bool oldUndulator = undulatorTracking_;
	bool rVal = SGMScanConfiguration::setUndulatorTracking(undulatorTracking);
	if(rVal && oldUndulator != undulatorTracking_){
		emit undulatorTrackingChanged(undulatorTracking_);
		emit trackingGroupChanged(trackingGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setMonoTracking(bool monoTracking){
	bool oldMono = monoTracking_;
	bool rVal = SGMScanConfiguration::setMonoTracking(monoTracking);
	if(rVal && oldMono != monoTracking_){
		emit undulatorTrackingChanged(monoTracking_);
		emit trackingGroupChanged(trackingGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setExitSlitTracking(bool exitSlitTracking){
	bool oldExitSlit = exitSlitTracking_;
	bool rVal = SGMScanConfiguration::setExitSlitTracking(exitSlitTracking);
	if(rVal && oldExitSlit != exitSlitTracking_){
		emit undulatorTrackingChanged(exitSlitTracking_);
		emit trackingGroupChanged(trackingGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setExitSlitGap(double exitSlitGap) {
	double oldExitSlit = exitSlitGap_;
	bool rVal = SGMScanConfiguration::setExitSlitGap(exitSlitGap);
	if(rVal && oldExitSlit != exitSlitGap_){
		emit fluxResolutionGroupChanged(fluxResolutionGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setGrating(SGMBeamlineInfo::sgmGrating grating) {
	SGMBeamlineInfo::sgmGrating oldGrating = grating_;
	bool rVal = SGMScanConfiguration::setGrating(grating);
	if(rVal && oldGrating != grating_){
		emit fluxResolutionGroupChanged(fluxResolutionGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setGrating(int grating) {
	return setGrating( (SGMBeamlineInfo::sgmGrating)grating );
}

bool SGMXASScanConfiguration::setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonic) {
	SGMBeamlineInfo::sgmHarmonic oldHarmonic = harmonic_;
	bool rVal = SGMScanConfiguration::setHarmonic(harmonic);
	if(rVal && oldHarmonic != harmonic_){
		emit fluxResolutionGroupChanged(fluxResolutionGroup_);
		setModified(true);
		emit configurationChanged();
	}
	return rVal;
}

bool SGMXASScanConfiguration::setHarmonic(int harmonic) {
	return setHarmonic( (SGMBeamlineInfo::sgmHarmonic)harmonic );
}

bool SGMXASScanConfiguration::setDetectorConfigurations(const AMOldDetectorInfoSet &xasDetectorsCfg){
	xasDetectorsCfg_ = xasDetectorsCfg;
	setModified(true);
	return true;
}


