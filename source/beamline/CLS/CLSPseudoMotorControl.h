#ifndef CLSPSEUDOMOTORCONTROL_H
#define CLSPSEUDOMOTORCONTROL_H

#include "beamline/AMPVControl.h"

/// This class adds an ability to delay a move because of underlying limitations of the CLS pseudo motor system.
class CLSPseudoMotorControl : public AMPVwStatusControl
{
	Q_OBJECT

public:
	/// Constructor.  Exactly the same as an AMPVwStatusControl.
	CLSPseudoMotorControl(const QString& name,
						   const QString& readPVname,
						   const QString& writePVname,
						   const QString& movingPVname,
						   const QString& stopPVname = QString(),
						   QObject* parent = 0,
						   double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
						   double moveStartTimeoutSeconds = 2.0,
						   AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
						   int stopValue = 1,
						   const QString &description = "");
	/// Destructor.
	virtual ~CLSPseudoMotorControl(){}

public slots:
	/// Starts a move.  If the control is already moving, it queues up a move to be started right after this one finishes.
	virtual FailureExplanation move(double setpoint);
	/// Tell the motor to stop.  This will also negate a delayed move if one is queued.  (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

protected slots:
	/// Re-implemented: This is used to handle when the movingPV_ changes.  Calls move() if a delay move is true.
	virtual void onMovingChanged(int isMovingValue);

protected:
	/// Flag for that says whether the pseudomotor has a delayed move.
	bool delayMove_;
	/// The setpoint for a delay move.
	double delayMoveSetpoint_;
};

#endif // CLSPSEUDOMOTORCONTROL_H
