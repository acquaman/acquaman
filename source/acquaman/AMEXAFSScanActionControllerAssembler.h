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


#ifndef AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H
#define AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

#define AMEXAFSSCANACTIONCONTROLLERASSEMBLER_AXES_INVALID 238000

/// Specific assembler for building an EXAFS scan controller.  Uses the specific AMScanAxisEXAFSRegion to flesh out the scan axis when switching to k-space.
class AMEXAFSScanActionControllerAssembler : public AMGenericScanActionControllerAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	AMEXAFSScanActionControllerAssembler(QObject *parent = 0);
	/// Destructor.
	virtual ~AMEXAFSScanActionControllerAssembler() {}

protected:
	/// Re-implemented for EXAFS scans.  Builds the action tree required to make an EXAFS scan.
	virtual bool generateActionTreeImplmentation();

	/// Method that builds an EXAFS region.
	AMAction3 *generateActionTreeForEXAFSStepAxisRegion(AMControl *axisControl, AMScanAxisEXAFSRegion *exafsRegion, bool isFinalRegion);
};

#endif // AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H
