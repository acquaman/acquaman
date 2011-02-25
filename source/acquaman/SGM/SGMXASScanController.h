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


#ifndef AMSGMXASSCANCONTROLLER_H
#define AMSGMXASSCANCONTROLLER_H

#include "beamline/SGM/SGMBeamline.h"
#include "SGMXASScanConfiguration.h"
#include "dataman/AMXASScan.h"

class SGMXASScanController
{
public:
	SGMXASScanController(SGMXASScanConfiguration *cfg);

	bool isBeamlineInitialized();
	virtual bool beamlineInitialize();
	virtual void reinitialize();

protected:
	SGMXASScanConfiguration *specificCfg_;
	AMBeamlineParallelActionsList *initializationActions_;
	bool beamlineInitialized_;
	AMXASScan *specificScan_;

private:
	SGMXASScanConfiguration **_pCfg_;
	AMXASScan **_pScan_;

	SGMXASScanConfiguration* pCfg_();
	AMXASScan* pScan_();
};

#endif // AMSGMXASSCANCONTROLLER_H
