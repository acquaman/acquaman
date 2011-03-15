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


#include "SGMScanConfiguration.h"

SGMScanConfiguration::SGMScanConfiguration()
{
	setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
	setGrating((SGMBeamline::sgm()->grating()->value() < 1) ? SGMBeamline::lowGrating : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::mediumGrating : SGMBeamline::highGrating));
	setHarmonic((SGMBeamline::sgm()->harmonic()->value() < 1) ? SGMBeamline::firstHarmonic : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::firstHarmonic : SGMBeamline::thirdHarmonic));

	setTrackingGroup(SGMBeamline::sgm()->trackingSet()->toInfoList());

	/* NTBA March 14, 2011 David Chevrier
	   Need something like setTrackingGroup for the detectorSet
	*/
}

bool SGMScanConfiguration::setTrackingGroup(AMControlInfoList trackingGroup){
	if(SGMBeamline::sgm()->trackingSet()->validInfoList(trackingGroup)){
		trackingGroup_ = trackingGroup;
		return true;
	}
	return false;
}

bool SGMScanConfiguration::setExitSlitGap(double exitSlitGap){
	if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
		return FALSE;
	exitSlitGap_ = exitSlitGap;
	return TRUE;
}
