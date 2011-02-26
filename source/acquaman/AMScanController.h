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


#ifndef ACQMAN_SCANCONTROLLER_H
#define ACQMAN_SCANCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "AMScanConfiguration.h"
#include "dataman/AMScan.h"

class AMScanController : public QObject
{
Q_OBJECT
Q_PROPERTY(bool running READ isRunning)
Q_PROPERTY(bool paused READ isPaused)

public:
	explicit AMScanController(AMScanConfiguration *configuration, QObject *parent = 0);

	/// Returns true if the scan is running but not paused
	virtual bool isRunning() const {return running_ && !paused_;}
	/// Convenience call, returns true if the scan is not running
	virtual bool isStopped() const {return !isRunning();}
	/// Returns true if the scan is running and paused
	virtual bool isPaused() const {return paused_;}
	virtual bool isInitialized() const {return initialized_;}
	virtual bool hasFailed() const { return failed_; }
	virtual bool hasFinished() const { return finished_; }

	virtual AMScan* scan() { return generalScan_; }

signals:
	// Implementations are responsible for emitting the following signals as events happen during the scan.

	/// Scan has initialized (ie: the beamline has been configured and moved into position, etc. Ready to start() the scan.)
	void initialized();
	/// Scan has started
	void started();
	/// Scan completed
	void finished();
	/// Scan canceled by user
	void cancelled();
	/// Scan paused
	void paused();
	/// Scan resumed from being paused
	void resumed();
	/// Scan failed (due to some reason out of the user's control)
	void failed();

	/// Time left in scan. Implementations should emit this periodically
	void timeRemaining(double seconds);
	/// Progress of scan (arbitrary units: some amount \c elapsed of a \c total amount). Implementations should emit this periodically.
	void progress(double elapsed, double total);




public slots:
	/// This function will be called to let a scan controller do any beamline or software initialization that is required. Implementations should emit the initialized() signal when complete.
	virtual void initialize() = 0;
	/// Start scan running if not currently running or paused
	virtual void start() = 0;
	/// Cancel scan if currently running or paused
	virtual void cancel() = 0;
	/// Pause scan if currently running
	virtual void pause() = 0;
	/// Resume scan if currently paused
	virtual void resume() = 0;




protected:
	/// Configuration for this scan
	AMScanConfiguration *generalCfg_;
	AMScan *generalScan_;
	/// Holds whether this scan is currently running
	bool running_;
	/// Holds whether this scan is currently paused
	bool paused_;
	bool initialized_;
	bool failed_;
	bool finished_;

private:
	// unused: AMScanConfiguration **_pCfg_;
	// unused: AMScan **_pScan_;

	// unused: AMScanConfiguration* pCfg_() { return *_pCfg_;}
	// unused: AMScan* pScan_() {return *_pScan_;}
};

class AMScanControllerSupervisor : public QObject
{
Q_OBJECT
public:
	static AMScanControllerSupervisor* scanControllerSupervisor();
	static void releaseScanControllerSupervisor();

	virtual ~AMScanControllerSupervisor();

	AMScanController* currentScanController();

public slots:
	bool setCurrentScanController(AMScanController *newController);
	bool deleteCurrentScanController();

signals:
	void currentScanControllerCreated();
	void currentScanControllerDestroyed();
//	void currentScanControllerReinitialized(bool removeScan);

protected slots:
	void onCurrentScanControllerFinished();
	// void onCurrentScanControllerReinitialized(bool removeScan);

protected:
	AMScanControllerSupervisor(QObject *parent = 0);
	static AMScanControllerSupervisor *instance_;

	AMScanController *currentScanController_;
};

#endif // ACQMAN_SCANCONTROLLER_H
