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


#ifndef VESPERSCCDBASICDETECTOREMULATOR_H
#define VESPERSCCDBASICDETECTOREMULATOR_H

#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/VESPERS/VESPERSCCDDetector.h"

/// This class only provides a means of having a dwell key set for the detector.
class VESPERSCCDBasicDetectorEmulator : public AMBasicControlDetectorEmulator
{
	Q_OBJECT

public:
	/// Constructor.  Provide everything the normal emulator requires but also the VESPERSCCD..
	VESPERSCCDBasicDetectorEmulator(VESPERSCCDDetector *ccd, const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod,  QObject *parent = 0);

	/// The cannot be configured in this manner
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns empty string
	virtual QString synchronizedDwellKey() const { return ccd_->synchronizedDwellKey();}

protected:
	/// This function is called from the Acquiring (acquisition) state when the implementation should start the detector's acquisition. Once the detector is done acquiring you must call setAcquisitionSucceeded or setAcquisitionFailed
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);

	/// The pointer to the CCD.
	VESPERSCCDDetector *ccd_;
};

#endif // VESPERSCCDBASICDETECTOREMULATOR_H
