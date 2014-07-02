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


#ifndef AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
#define AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H

#include "acquaman/AMScanActionControllerScanOptimizer.h"

class AMDetectorTriggerSourceScanOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
 	virtual ~AMDetectorTriggerSourceScanOptimizer();
	AMDetectorTriggerSourceScanOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);
};

#endif // AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
