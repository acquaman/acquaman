#ifndef SGMROUGHINGPUMP_H
#define SGMROUGHINGPUMP_H

#include "beamline/AMConnectedControl.h"

class SGMRoughingPumpRunningStatus;

class SGMRoughingPump : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	SGMRoughingPump(const QString &name, const QString &baseName, QObject *parent);
	/// Destructor.
	virtual ~SGMRoughingPump();

	/// Returns the running status.
	SGMRoughingPumpRunningStatus* runningStatus() const { return running_; }

protected:
	/// The running status.
	SGMRoughingPumpRunningStatus *running_;
};

#endif // SGMROUGHINGPUMP_H
