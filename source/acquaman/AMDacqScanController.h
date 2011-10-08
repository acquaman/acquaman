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


#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

class QTime;
class QStringList;
class QDir;

#include "util/AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "AMAcqScanSpectrumOutput.h"
#include "dacq3_2/qepicsadvacq.h"

#include "dataman/AMnDIndex.h"

#include "beamline/AMDetector.h"

class AMDacqScanController : public AMScanController
{
Q_OBJECT
public:
	AMDacqScanController(AMScanConfiguration *cfg, QObject *parent = 0);

	/// Setup the controls to be used for changing dwell time between regions. Short documentation in the CPP.
	void useDwellTimes(AMControl *dwellTimeTrigger, AMControl *dwellTimeConfirmed);

protected:
	bool startImplementation();
	bool canPause();
	void pauseImplementation();
	void resumeImplementation();
	void cancelImplementation();

protected:
	/// Convert the AMDetector::ReadMethod to the enum list used by the dacq library
	int detectorReadMethodToDacqReadMethod(AMDetector::ReadMethod readMethod);

	bool event(QEvent *e);
	virtual AMnDIndex toScanIndex(QMap<int, double> aeData);

protected slots:
	virtual void onDacqStart();
	virtual void onDacqStop();
	virtual void onDacqPause(int mode);
	virtual void onDacqSendCompletion(int completion);
	virtual void onDacqState(const QString& state);

	/// Virtual function to deal with dwell time changes between regions. Can be re-implemented in subclasses for particular behavior.
	virtual void onDwellTimeTriggerChanged(double newValue);

protected:
	QEpicsAdvAcq *advAcq_;
	bool usingSpectraDotDatFile_;
	bool dacqCancelled_;
	QTime startTime_;

	/// Controls for triggering and confirming dwell time changes between regions (as well as bool for backwards compatibility)
	bool useDwellTimes_;
	AMControl *dwellTimeTrigger_;
	AMControl *dwellTimeConfirmed_;

private:
	AMScanConfiguration **_pCfg_;
	AMScan **_pScan_;

	AMScanConfiguration *pCfg_() { return *_pCfg_;}
	AMScan *pScan_() { return *_pScan_;}
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
