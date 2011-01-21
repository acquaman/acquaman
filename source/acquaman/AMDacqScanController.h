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


#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include <QTime>
#include <QStringList>
#include <QDir>

#include "AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "AMAcqScanSpectrumOutput.h"
#include "dacq3_2/qepicsadvacq.h"

#include "dataman/AMnDIndex.h"

class AMDacqScanController : public AMScanController
{
Q_OBJECT
public:
	AMDacqScanController(AMScanConfiguration *cfg, QObject *parent = 0);

public slots:
//    /// Sets a new scan configuration
//    virtual void newConfigurationLoad(AMScanConfiguration &cfg);
	/// Start scan running if not currently running or paused
	virtual void start();
	/// Cancel scan if currently running or paused
	virtual void cancel();
	/// Pause scan if currently running
	virtual void pause() { advAcq_->Pause(1);}
	/// Resume scan if currently paused
	virtual void resume(){ advAcq_->Pause(0);}

protected:
	bool event(QEvent *e);
	virtual AMnDIndex toScanIndex(QMap<int, double> aeData);

protected:
	QEpicsAdvAcq *advAcq_;
	bool cancelled_;
	QTime startTime_;

protected slots:
	void onStart();
	void onStop();
	void onPause(int mode);
	void onSendCompletion(int completion);

private:
	AMScanConfiguration **_pCfg_;
	AMScan **_pScan_;

	AMScanConfiguration *pCfg_() { return *_pCfg_;}
	AMScan *pScan_() { return *_pScan_;}
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
