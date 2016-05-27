#ifndef BIOXASMAXVMOTORMOVETOLIMITACTIONINFO_H
#define BIOXASMAXVMOTORMOVETOLIMITACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASMAXvMotorMoveToLimitActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
	Q_PROPERTY(CLSMAXvMotor::Limit limitSetpoint READ limitSetpoint WRITE setLimitSetpoint)

public:
	/// Constructor.
	Q_INVOKABLE BioXASMAXvMotorMoveToLimitActionInfo(const AMControlInfo &control, CLSMAXvMotor::Limit limit, QObject *parent = 0);
	/// Copy constructor.
	BioXASMAXvMotorMoveToLimitActionInfo(const BioXASMAXvMotorMoveToLimitActionInfo &original);
	/// Destructor.
	virtual ~BioXASMAXvMotorMoveToLimitActionInfo();

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3* createCopy() const;

	/// Returns the action description.
	virtual QString typeDescription() const { return "Motor move to limit"; }
	/// Returns the motor being moved.
	const AMControlInfo& controlInfo() const { return controlInfo_; }
	/// Returns the limit setpoint.
	CLSMAXvMotor::Limit limitSetpoint() const { return limitSetpoint_; }

	/// Returns a string representation of the given limit setpoint.
	QString limitSetpointToString(CLSMAXvMotor::Limit limit) const;

public slots:
	/// Sets the motor being moved.
	void setControlInfo(const AMControlInfo &info);
	/// Sets the limit setpoint.
	void setLimitSetpoint(CLSMAXvMotor::Limit setpoint);

protected slots:
	/// Handles updating the description.
	void updateDescriptionText();

protected:
	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The motor being moved.
	AMControlInfo controlInfo_;
	/// The limit setpoint.
	CLSMAXvMotor::Limit limitSetpoint_;
};

#endif // BIOXASMAXVMOTORMOVETOLIMITACTIONINFO_H
