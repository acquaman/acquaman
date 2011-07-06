/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef SGMFASTSCANCONTROLLER_H
#define SGMFASTSCANCONTROLLER_H

#include "dataman/AMFastScan.h"
#include "SGMFastScanConfiguration.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMUser.h"

class SGMFastScanController
{
public:
	SGMFastScanController(SGMFastScanConfiguration *cfg);

	bool isBeamlineInitialized();
	virtual bool beamlineInitialize();

protected:
	SGMFastScanConfiguration *specificCfg_;
	AMBeamlineParallelActionsList *initializationActions_;
	AMBeamlineParallelActionsList *cleanUpActions_;
	bool beamlineInitialized_;
	AMFastScan *specificScan_;

private:
	SGMFastScanConfiguration* pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};

#endif // SGMFASTSCANCONTROLLER_H
