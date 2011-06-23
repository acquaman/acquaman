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


#ifndef ACQMAN_SGMXASDACQSCANCONTROLLER_H
#define ACQMAN_SGMXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "SGMXASScanController.h"

#define SGMXASDACQSCANCONTROLLER_CANT_INTIALIZE 27001
#define SGMXASDACQSCANCONTROLLER_CANT_START_BL_SCANNING 27002
#define SGMXASDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 27003
#define SGMXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 27004

class SGMXASDacqScanController : public AMDacqScanController, public SGMXASScanController
{
Q_OBJECT
public:
	explicit SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

	virtual AMScan* scan() {return pScan_();}

protected:
	bool initializeImplementation();
	bool startImplementation();
	AMnDIndex toScanIndex(QMap<int, double> aeData);

protected slots:
	// Re-implementing to intercept finished() signal and do cleanup
	void onDacqStop();

	void onInitializationActionsSucceeded();
	void onInitializationActionsFailed(int explanation);
	void onInitializationActionsProgress(double elapsed, double total);

	void onScanFinished();

private:
	/// \todo Why the double pointers?
	SGMXASScanConfiguration **_pCfg_;
	/// \todo Why the double pointer system?
	AMXASScan **_pScan_;

	SGMXASScanConfiguration *pCfg_() { return *_pCfg_;}
	AMXASScan* pScan_() { return *_pScan_;}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
