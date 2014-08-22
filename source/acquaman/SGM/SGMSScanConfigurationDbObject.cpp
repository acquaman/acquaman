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


#include "SGMSScanConfigurationDbObject.h"

SGMSScanConfigurationDbObject::SGMSScanConfigurationDbObject(QObject *parent) :
	AMDbObject(parent)
{
}

SGMSScanConfigurationDbObject::~SGMSScanConfigurationDbObject(){}

SGMSScanConfigurationDbObject::SGMSScanConfigurationDbObject(const SGMSScanConfigurationDbObject &original) :
	AMDbObject(original)
{
	trackingGroup_ = original.trackingGroup();
	fluxResolutionGroup_ = original.fluxResolutionGroup();
}

bool SGMSScanConfigurationDbObject::monoTracking() const{
	if( fabs(trackingGroup_.controlNamed("monoTracking").value() - 1.0) < trackingGroup_.controlNamed("monoTracking").tolerance() )
		return true;
	return false;
}

bool SGMSScanConfigurationDbObject::undulatorTracking() const{
	if( fabs(trackingGroup_.controlNamed("undulatorTracking").value() - 1.0) < trackingGroup_.controlNamed("undulatorTracking").tolerance() )
		return true;
	return false;
}

bool SGMSScanConfigurationDbObject::exitSlitTracking() const{
	if( fabs(trackingGroup_.controlNamed("exitSlitTracking").value() - 1.0) < trackingGroup_.controlNamed("exitSlitTracking").tolerance() )
		return true;
	return false;
}

double SGMSScanConfigurationDbObject::exitSlitGap() const{
	return fluxResolutionGroup_.controlNamed("exitSlitGap").value();
}

SGMBeamlineInfo::sgmGrating SGMSScanConfigurationDbObject::grating() const{
	return (SGMBeamlineInfo::sgmGrating)(fluxResolutionGroup_.controlNamed("grating").value());
}

SGMBeamlineInfo::sgmHarmonic SGMSScanConfigurationDbObject::harmonic() const{
	return (SGMBeamlineInfo::sgmHarmonic)(fluxResolutionGroup_.controlNamed("harmonic").value());
}

void SGMSScanConfigurationDbObject::setTrackingGroup(const AMControlInfoList &trackingGroup){
	if(trackingGroup_ != trackingGroup){
		trackingGroup_ = trackingGroup;
		emit trackingGroupChanged();
		emit monoTrackingChanged(monoTracking());
		emit undulatorTrackingChanged(undulatorTracking());
		emit exitSlitTrackingChanged(exitSlitTracking());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setMonoTracking(bool monoIsTracking){
	if(monoTracking() != monoIsTracking){
		if(monoIsTracking)
			trackingGroup_.controlNamed("monoTracking").setValue(1.0);
		else
			trackingGroup_.controlNamed("monoTracking").setValue(0.0);
		emit trackingGroupChanged();
		emit monoTrackingChanged(monoTracking());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setUndulatorTracking(bool undulatorIsTracking){
	if(undulatorTracking() != undulatorIsTracking){
		if(undulatorIsTracking)
			trackingGroup_.controlNamed("undulatorTracking").setValue(1.0);
		else
			trackingGroup_.controlNamed("undulatorTracking").setValue(0.0);
		emit trackingGroupChanged();
		emit undulatorTrackingChanged(undulatorTracking());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setExitSlitTracking(bool exitSlitIsTracking){
	if(exitSlitTracking() != exitSlitIsTracking){
		if(exitSlitIsTracking)
			trackingGroup_.controlNamed("exitSlitTracking").setValue(1.0);
		else
			trackingGroup_.controlNamed("exitSlitTracking").setValue(0.0);
		emit trackingGroupChanged();
		emit exitSlitTrackingChanged(exitSlitTracking());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setFluxResolutionGroup(const AMControlInfoList &fluxResolutionGroup){
	if(fluxResolutionGroup_ != fluxResolutionGroup){
		fluxResolutionGroup_ = fluxResolutionGroup;
		emit fluxResolutionGroupChanged();
		emit exitSlitGapChanged(exitSlitGap());
		emit gratingChanged(grating());
		emit harmonicChanged(harmonic());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setExitSlitGap(double exitSlitGapValue){
	if(exitSlitGap() != exitSlitGapValue){
		fluxResolutionGroup_.controlNamed("exitSlitGap").setValue(exitSlitGapValue);
		emit fluxResolutionGroupChanged();
		emit exitSlitGapChanged(exitSlitGap());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setGrating(SGMBeamlineInfo::sgmGrating gratingValue){
	if(grating() != gratingValue){
		fluxResolutionGroup_.controlNamed("grating").setValue(gratingValue);
		emit fluxResolutionGroupChanged();
		emit gratingChanged(grating());
		setModified(true);
	}
}

void SGMSScanConfigurationDbObject::setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonicValue){
	if(harmonic() != harmonicValue){
		fluxResolutionGroup_.controlNamed("harmonic").setValue(harmonicValue);
		emit fluxResolutionGroupChanged();
		emit harmonicChanged(harmonic());
		setModified(true);
	}
}

