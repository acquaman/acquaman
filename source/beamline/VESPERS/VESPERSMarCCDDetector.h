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


#ifndef VESPERSMARCCDDETECTOR_H
#define VESPERSMARCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

/*!
  This class encapsulates the Mar CCD used on VESPERS.  Although currently not available, it will have the current image set available for viewing.
  What it will offer at the moment is the ability to control the important aspects of the detector, such as the temperature, accumulationn time,
  starting, stopping, etc.
  */
class VESPERSMarCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~VESPERSMarCCDDetector();
	VESPERSMarCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;
};

#endif // VESPERSMARCCDDETECTOR_H
