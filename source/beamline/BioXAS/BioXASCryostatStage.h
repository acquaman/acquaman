#ifndef BIOXASCRYOSTATSTAGE_H
#define BIOXASCRYOSTATSTAGE_H

#include "beamline/AMMotorGroup.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASCryostatStage : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCryostatStage(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCryostatStage();

	/// Returns the motor group.
	virtual AMMotorGroup *motorGroup() const = 0;
};

#endif // BIOXASCRYOSTATSTAGE_H
