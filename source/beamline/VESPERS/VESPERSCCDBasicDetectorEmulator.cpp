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


#include "VESPERSCCDBasicDetectorEmulator.h"

#include "actions3/actions/AMDetectorInitializeAction.h"
#include "actions3/actions/AMDetectorDwellTimeAction.h"
#include "actions3/actions/AMDetectorAcquisitionAction.h"
#include "actions3/actions/AMDetectorTriggerAction.h"
#include "actions3/actions/AMDetectorReadAction.h"
#include "actions3/actions/AMDetectorCleanupAction.h"

VESPERSCCDBasicDetectorEmulator::VESPERSCCDBasicDetectorEmulator(VESPERSCCDDetector *ccd, const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, QObject *parent)
	: AMBasicControlDetectorEmulator(name, description, control, statusControl, statusAcquiringValue, statusNotAcquiringValue, readMethod, parent)
{
	ccd_ = ccd;
}

bool VESPERSCCDBasicDetectorEmulator::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	return ccd_->acquire(readMode);
}

AMAction3* VESPERSCCDBasicDetectorEmulator::createSetAcquisitionTimeAction(double seconds){
	return ccd_->createSetAcquisitionTimeAction(seconds);
}

AMAction3* VESPERSCCDBasicDetectorEmulator::createAcquisitionAction(AMDetectorDefinitions::ReadMode readMode){
	return ccd_->createAcquisitionAction(readMode);
}

AMAction3* VESPERSCCDBasicDetectorEmulator::createTriggerAction(AMDetectorDefinitions::ReadMode readMode){
	return ccd_->createTriggerAction(readMode);
}
