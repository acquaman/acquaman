#ifndef BIOXASPSEUDOMOTORCONTROL_H
#define BIOXASPSEUDOMOTORCONTROL_H

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSPseudoMotorControl.h"

class BioXASPseudoMotorControl : public CLSPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.  added pbBaseName, enablePVname.
	BioXASPseudoMotorControl(const QString& name,
							 const QString& pvBaseName,
							 const QString& readPVname,
							 const QString& writePVname,
							 const QString& movingPVname,
							 const QString& enabledPVname,
							 const QString& stopPVname = QString(),
							 double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
							 double moveStartTimeoutSeconds = 2.0,
							 AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
							 int stopValue = 1,
							 const QString &description = "",
							 QObject* parent = 0);
	/// Destructor.
	virtual ~BioXASPseudoMotorControl(){}

	/// Returns the PV base name
	QString pvBaseName() const;
	/// Returns the pv name of the MAXvMotor enabled PV (:enabled)
	QString enabledPVName() const;
	/// Returns the pv name of the MAXvMotor status PV (:status)
	QString statusPVName() const;

	/// Returns the status PV control, which can be used as the statusTagControl for control editor
	AMReadOnlyPVControl *statusPVControl();

	/// Indicates that this control currently can cause moves:
	virtual bool canMove() const;

signals:
	/// the signale of Enabled PV value changed
	void enabledPVValueChanged(double);

protected:
	/// the base name of the pseudo motor
	QString pvBaseName_;

	/// Readonly contorl for PV status, which can be used as the statusTagControl for control editor
	AMReadOnlyPVControl *statusPVControl_;
	/// Readonly contorl for pseudo motor enabled PV
	AMReadOnlyPVControl *enabledPVControl_;
};

#endif // BIOXASPSEUDOMOTORCONTROL_H
