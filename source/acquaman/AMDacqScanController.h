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


#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

class QTime;
class QStringList;
class QDir;

#include "util/AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_3/OutputHandler/acqFactory.h"
#include "AMAcqScanSpectrumOutput.h"
#include "dacq3_3/qepicsadvacq.h"

#include "dataman/AMnDIndex.h"

#include "beamline/AMOldDetector.h"

#define AMDACQSCANCONTROLLER_CANT_CREATE_OUTPUTHANDLER 72001
#define AMDACQSCANCONTROLLER_DACQ_INITIALIZATION_FAILED 72002
#define AMDACQSCANCONTROLLER_NO_X_COLUMN 72003
#define AMDACQSCANCONTROLLER_NO_SPECTRUM_FILE 72004

class AMDacqScanController : public AMScanController
{
Q_OBJECT
public:
	AMDacqScanController(AMScanConfiguration *cfg, QObject *parent = 0);

	/// Setup the controls to be used for changing dwell time between regions. Short documentation in the CPP.
	void useDwellTimes(AMControl *dwellTimeTrigger, AMControl *dwellTimeConfirmed);
	/// Returns whether the controller is going to stop immediately.
	bool stoppingImmediately() const { return stopImmediately_; }

public slots:
	/// Tells the controller to stop.  The controller will finish what it is currently doing and then stop.
	void stopImmediately() { stopImmediately_ = true; }

protected:
	bool startImplementation();
	bool canPause() const;
	void pauseImplementation();
	void resumeImplementation();
	void cancelImplementation();

protected:
	/// Convert the AMDetector::ReadMethod to the enum list used by the dacq library
	int detectorReadMethodToDacqReadMethod(AMOldDetector::ReadMethod readMethod);

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
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();

protected:
	QEpicsAdvAcq *advAcq_;
	bool usingSpectraDotDatFile_;
	bool dacqCancelled_;
	QTime startTime_;

	/// Flag used to determine whether the scan should be stopped.  This is stopping before the end of the scan, but not cancelling the scan.
	bool stopImmediately_;

	/// Controls for triggering and confirming dwell time changes between regions (as well as bool for backwards compatibility)
	bool useDwellTimes_;
	AMControl *dwellTimeTrigger_;
	AMControl *dwellTimeConfirmed_;

	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
