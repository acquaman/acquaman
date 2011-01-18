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
	explicit AMScanController(AMScanConfiguration *cfg, QObject *parent = 0);

	/// Returns true if the scan is running but not paused
	bool isRunning() const {return running_ && !paused_;}
	/// Convenience call, returns true if the scan is not running
	bool isStopped() const {return !isRunning();}
	/// Returns true if the scan is running and paused
	bool isPaused() const {return paused_;}
	bool isInitialized() const {return initialized_;}

	virtual AMScan* scan() {return pScan_();}

signals:
	/// Scan has started
	void started();
	/// Scan completed
	void finished();
	/// Scan canceled by user
	void cancelled();
	/// Scan paused
	void paused();
	/// Scan resumed
	void resumed();
	/// Time left in scan
	void timeRemaining(double seconds);
	void progress(double elapsed, double total);

	void scanCreated(AMScan *scan);
	void reinitialized(bool removeScan);

public slots:
	/// Start scan running if not currently running or paused
	virtual void start() = 0;
	/// Cancel scan if currently running or paused
	virtual void cancel() = 0;
	/// Pause scan if currently running
	virtual void pause() = 0;
	/// Resume scan if currently paused
	virtual void resume() = 0;
	virtual void initialize() = 0;

protected:
	/// Configuration for this scan
	AMScanConfiguration *generalCfg_;
	AMScan *generalScan_;
	/// Holds whether this scan is currently running
	bool running_;
	/// Holds whether this scan is currently paused
	bool paused_;
	bool initialized_;

private:
	AMScanConfiguration **_pCfg_;
	AMScan **_pScan_;

	AMScanConfiguration* pCfg_() { return *_pCfg_;}
	AMScan* pScan_() {return *_pScan_;}
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
	void currentScanControllerReinitialized(bool removeScan);

protected slots:
	void onCurrentScanControllerFinished();
	void onCurrentScanControllerReinitialized(bool removeScan);

protected:
	AMScanControllerSupervisor(QObject *parent = 0);
	static AMScanControllerSupervisor *instance_;

	AMScanController *currentScanController_;
};

#endif // ACQMAN_SCANCONTROLLER_H
