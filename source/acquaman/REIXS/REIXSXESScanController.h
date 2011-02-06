#ifndef REIXSXESSCANCONTROLLER_H
#define REIXSXESSCANCONTROLLER_H

#include "acquaman/AMScanController.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

/// This is the implementation of AMScanController that executes a REIXS XES Scan, based on the parameters in a given REIXSXESScanConfiguration.
class REIXSXESScanController : public AMScanController
{
	Q_OBJECT
public:
	explicit REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

signals:

public slots:
	/// Start scan running if not currently running or paused
	virtual void start() {}
	/// Cancel scan if currently running or paused
	virtual void cancel() {}
	/// Pause scan if currently running
	virtual void pause() {}
	/// Resume scan if currently paused
	virtual void resume() {}
	/// Called before starting to satisfy any prerequisites (ie: setting up the beamline, setting up files, etc.)
	virtual void initialize() {}

	/// Called when a scan is re-initialized. \todo I don't know exactly when or what this means, or what removeScan should do.
	virtual void reinitialize(bool removeScan) {}

};

#endif // REIXSXESSCANCONTROLLER_H
