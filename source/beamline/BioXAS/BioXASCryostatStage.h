#ifndef BIOXASCRYOSTATSTAGE_H
#define BIOXASCRYOSTATSTAGE_H

#include "beamline/AMMotorGroup.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASCryostatStage : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCryostatStage(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCryostatStage();

	/// Returns true if the cryostat stage is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the motor group.
	virtual AMMotorGroup *motorGroup() const { return cryostatStageMotors_; }
	/// Returns the stage x motor.
	virtual CLSMAXvMotor *stageXControl() const { return cryostatX_; }
	/// Returns the stage y motor.
	virtual CLSMAXvMotor *stageYControl() const { return cryostatY_; }
	/// Returns the stage z motor.
	virtual CLSMAXvMotor* stageZControl() const { return cryostatZ_; }

protected:
	/// Cryostat stage motor group.
	AMMotorGroup *cryostatStageMotors_;

	/// Cryostat stage x motor.
	CLSMAXvMotor *cryostatX_;
	/// Cryostat stage y motor.
	CLSMAXvMotor *cryostatY_;
	/// Cryostat stage z motor.
	CLSMAXvMotor *cryostatZ_;
};

#endif // BIOXASCRYOSTATSTAGE_H
