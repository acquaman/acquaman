/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSROPERCCDDETECTOR_H
#define VESPERSROPERCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

class VESPERSRoperQRunnableImageLoader;

/*!
  This class encapsulates the Roper CCD used on VESPERS.  It is now possible to view an image.
  What it will offer at the moment is the ability to control the important aspects of the detector, (eg. accumulationn time),
  starting, stopping, etc.
  */
class VESPERSRoperCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSRoperCCDDetector();

	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;
};

#endif // VESPERSROPERCCDDETECTOR_H
