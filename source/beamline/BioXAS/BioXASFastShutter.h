#ifndef BIOXASFASTSHUTTER_H
#define BIOXASFASTSHUTTER_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class BioXASFastShutter : public AMExclusiveStatesEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFastShutter(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFastShutter();

signals:
	/// Notifier that the shutter status control has changed.
	void statusChanged(AMControl *newStatus);
	/// Notifier that the shutter states have changed.
	void statesChanged();

public slots:
	/// Sets the shutter status control.
	void setStatus(AMControl *newStatus);
	/// Adds a shutter state.
	void addShutterState(int index, const QString &stateName, AMControl *control, double triggerValue);
	/// Removes a shutter state.
	void removeShutterState(int index);
	/// Clears all shutter states.
	void clearShutterStates();
};

#endif // BIOXASFASTSHUTTER_H
