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


#include "SGMScanConfiguration.h"

 SGMScanConfiguration::~SGMScanConfiguration(){}
SGMScanConfiguration::SGMScanConfiguration()
{
	exitSlitGap_ = -1;
	grating_ = SGMBeamlineInfo::lowGrating;
	harmonic_ = SGMBeamlineInfo::firstHarmonic;

	setTrackingGroup(SGMBeamline::sgm()->trackingSet()->toInfoList());
	setFluxResolutionGroup(SGMBeamline::sgm()->fluxResolutionSet()->toInfoList());

	/* NTBA March 14, 2011 David Chevrier
	   Need something like setTrackingGroup for the detectorSet
	*/
}

bool SGMScanConfiguration::undulatorTracking() const{
	return undulatorTracking_;
}

bool SGMScanConfiguration::monoTracking() const{
	return monoTracking_;
}

bool SGMScanConfiguration::exitSlitTracking() const{
	return exitSlitTracking_;
}

double SGMScanConfiguration::exitSlitGap() const{
	return exitSlitGap_;
}

SGMBeamlineInfo::sgmGrating SGMScanConfiguration::grating() const{
	return grating_;
}

SGMBeamlineInfo::sgmHarmonic SGMScanConfiguration::harmonic() const{
	return harmonic_;
}

AMControlInfoList SGMScanConfiguration::trackingGroup() const{
	return trackingGroup_;
}

AMControlInfoList SGMScanConfiguration::fluxResolutionGroup() const{
	return fluxResolutionGroup_;
}

bool SGMScanConfiguration::setTrackingGroup(AMControlInfoList trackingGroup){
	if(SGMBeamline::sgm()->trackingSet()->validInfoList(trackingGroup)){
		trackingGroup_ = trackingGroup;
		for(int x = 0; x < trackingGroup_.count(); x++){
			if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->undulatorTracking()->name() && trackingGroup_.at(x).value() != undulatorTracking_)
				undulatorTracking_ = trackingGroup_.at(x).value();
			if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->monoTracking()->name() && trackingGroup_.at(x).value() != monoTracking_)
				monoTracking_ = trackingGroup_.at(x).value();
			if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->exitSlitTracking()->name() && trackingGroup_.at(x).value() != exitSlitTracking_)
				exitSlitTracking_ = trackingGroup_.at(x).value();
		}
		return true;
	}
	return false;
}

bool SGMScanConfiguration::setFluxResolutionGroup(AMControlInfoList fluxResolutionGroup){
	if(SGMBeamline::sgm()->fluxResolutionSet()->validInfoList(fluxResolutionGroup)){
		fluxResolutionGroup_ = fluxResolutionGroup;
		for(int x = 0; x < fluxResolutionGroup_.count(); x++){
			if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->exitSlitGap()->name() && fluxResolutionGroup_.at(x).value() != exitSlitGap_)
				exitSlitGap_ = fluxResolutionGroup_.at(x).value();
			if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->grating()->name() && fluxResolutionGroup_.at(x).value() != grating_)
				grating_ = (SGMBeamlineInfo::sgmGrating)(fluxResolutionGroup_.at(x).value());
			if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->harmonic()->name() && fluxResolutionGroup_.at(x).value() != harmonic_)
				harmonic_ = (SGMBeamlineInfo::sgmHarmonic)(fluxResolutionGroup_.at(x).value());
		}
		return true;
	}
	return false;
}

bool SGMScanConfiguration::setExitSlitGap(double exitSlitGap){
	if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
		return false;
	exitSlitGap_ = exitSlitGap;
	for(int x = 0; x < fluxResolutionGroup_.count(); x++)
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->exitSlitGap()->name() && fluxResolutionGroup_.at(x).value() != exitSlitGap_)
			fluxResolutionGroup_[x].setValue(exitSlitGap_);
	return true;
}

bool SGMScanConfiguration::setGrating(SGMBeamlineInfo::sgmGrating grating){
	grating_ = grating;
	for(int x = 0; x < fluxResolutionGroup_.count(); x++)
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->grating()->name() && fluxResolutionGroup_.at(x).value() != grating_)
			fluxResolutionGroup_[x].setValue(grating_);
	return true;
}

bool SGMScanConfiguration::setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonic){
	harmonic_ = harmonic;
	for(int x = 0; x < fluxResolutionGroup_.count(); x++)
		if(fluxResolutionGroup_.at(x).name() == SGMBeamline::sgm()->harmonic()->name() && fluxResolutionGroup_.at(x).value() != harmonic_)
			fluxResolutionGroup_[x].setValue(harmonic_);
	return true;
}

bool SGMScanConfiguration::setUndulatorTracking(bool undulatorTracking){
	undulatorTracking_ = undulatorTracking;
	for(int x = 0; x < trackingGroup_.count(); x++)
		if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->undulatorTracking()->name() && trackingGroup_.at(x).value() != undulatorTracking_)
			trackingGroup_[x].setValue(undulatorTracking_);
	return true;
}

bool SGMScanConfiguration::setMonoTracking(bool monoTracking){
	monoTracking_ = monoTracking;
	for(int x = 0; x < trackingGroup_.count(); x++)
		if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->monoTracking()->name() && trackingGroup_.at(x).value() != monoTracking_)
			trackingGroup_[x].setValue(monoTracking_);
	return true;
}

bool SGMScanConfiguration::setExitSlitTracking(bool exitSlitTracking){
	exitSlitTracking_ = exitSlitTracking;
	for(int x = 0; x < trackingGroup_.count(); x++)
		if(trackingGroup_.at(x).name() == SGMBeamline::sgm()->exitSlitTracking()->name() && trackingGroup_.at(x).value() != exitSlitTracking_)
			trackingGroup_[x].setValue(exitSlitTracking_);
	return true;
}
