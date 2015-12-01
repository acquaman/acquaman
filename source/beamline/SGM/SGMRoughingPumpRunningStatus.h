#ifndef SGMROUGHINGPUMPRUNNINGSTATUS_H
#define SGMROUGHINGPUMPRUNNINGSTATUS_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class SGMRoughingPumpRunningStatus : public AMExclusiveStatesEnumeratedControl
{
	Q_OBJECT

public:
	/// Enum describing the possible running states.
	enum State { Running = 0, NotRunning = 1 };

	/// Constructor.
	SGMRoughingPumpRunningStatus(const QString &name, const QString &baseName, QObject *parent);
	/// Destructor.
	virtual ~SGMRoughingPumpRunningStatus();

protected:
	/// Adds a running state.
	void addRunningState(int index, const QString &stateName, AMControl *control);
};

#endif // SGMROUGHINGPUMPRUNNINGSTATUS_H
