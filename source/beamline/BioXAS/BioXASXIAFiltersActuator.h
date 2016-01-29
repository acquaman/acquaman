#ifndef BIOXASXIAFILTERSACTUATOR_H
#define BIOXASXIAFILTERSACTUATOR_H

#include "beamline/BioXAS/BioXASXIAFiltersActuatorControl.h"

class BioXASXIAFiltersActuator : public BioXASXIAFiltersActuatorControl
{
    Q_OBJECT

public:
	/// Enum describing the different actuator states.
	enum State { Out = 0, In = 1 };

	/// Constructor.
	explicit BioXASXIAFiltersActuator(const QString &deviceName, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersActuator();

	/// Returns a string representation of the given state value.
	static QString stateToString(double state);
};

#endif // BIOXASXIAFILTERSACTUATOR_H
