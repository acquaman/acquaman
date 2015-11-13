#ifndef BIOXASXIAFILTERSACTUATOR_H
#define BIOXASXIAFILTERSACTUATOR_H

#include "beamline/BioXAS/BioXASXIAFiltersActuatorControl.h"

class BioXASXIAFiltersActuator : public BioXASXIAFiltersActuatorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXIAFiltersActuator(const QString &deviceName, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersActuator();
};

#endif // BIOXASXIAFILTERSACTUATOR_H
