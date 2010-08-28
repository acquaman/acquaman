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

	static AMScanController* currentScanController();
	static bool setCurrentScanController(AMScanController *newScanController);

	/// Returns true if the scan is running but not paused
	bool isRunning() const {return running_ && !paused_;}
	/// Convenience call, returns true if the scan is not running
	bool isStopped() const {return !isRunning();}
	/// Returns true if the scan is running and paused
	bool isPaused() const {return paused_;}
	bool isInitialized() const {return initialized_;}

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

	/// Don't call these
	void currentScanControllerCreated();
	void currentScanControllerDestroyed();

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

protected slots:
	void initiateCurrentScanControllerCreated();
	void initiateCurrentScanControllerDestroyed();
	void onCurrentScanControllerFinished();

protected:
	static AMScanController *currentScanController_;
//	static void onCurrentScanControllerFinished();

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

#endif // ACQMAN_SCANCONTROLLER_H
