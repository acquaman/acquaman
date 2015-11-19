#ifndef SGMTURBOPUMP_H
#define SGMTURBOPUMP_H

#include "beamline/SGM/SGMControl.h"
#include "beamline/AMPVControl.h"

class SGMTurboPump : public SGMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMTurboPump(const QString &deviceName, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMTurboPump();

	/// Returns the running state control.
	AMSinglePVControl* running() const { return running_; }

protected:
	/// The running state control.
	AMSinglePVControl *running_;
};

#endif // SGMTURBOPUMP_H
