#ifndef BIOXASSIDECRYOSTATSTAGE_H
#define BIOXASSIDECRYOSTATSTAGE_H

#include "beamline/BioXAS/BioXASCryostatStage.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASSideCryostatStage : public BioXASCryostatStage
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideCryostatStage(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCryostatStage();

	/// Returns true if the cryostat stage is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the motor group.
	virtual AMMotorGroup *motorGroup() const { return cryostatStageMotors_; }
	/// Returns the stage x motor.
	CLSMAXvMotor *stageXControl() const { return cryostatX_; }
	/// Returns the stage y motor.
	CLSMAXvMotor *stageYControl() const { return cryostatY_; }
	/// Returns the stage z motor.
	CLSMAXvMotor* stageZControl() const { return cryostatZ_; }

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

#endif // BIOXASSIDECRYOSTATSTAGE_H
