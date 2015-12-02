#ifndef SGMTURBOPUMP_H
#define SGMTURBOPUMP_H

#include "source/beamline/AMConnectedControl.h"
#include "source/beamline/AMPVControl.h"

class SGMTurboPump : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Enum describing different running states.
	enum State { NotRunning = 0, Running = 1 };

	/// Constructor.
	explicit SGMTurboPump(const QString &deviceName, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMTurboPump();

	/// Returns the running state control.
	AMSinglePVControl* running() const { return running_; }

	/// Returns true if the turbo is running, false otherwise.
	bool isRunning() const;

protected:
	/// The running state control.
	AMSinglePVControl *running_;
};

#endif // SGMTURBOPUMP_H
