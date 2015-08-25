#include "SGMHexapodTransformedAxis.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include <QDebug>
SGMHexapodTransformedAxis::SGMHexapodTransformedAxis(AxisDesignation axis,
													 AMControl* globalXAxisSetpoint,
													 AMControl* globalXAxisFeedback,
													 AMControl* globalYAxisSetpoint,
													 AMControl* globalYAxisFeedback,
													 AMControl* globalZAxisSetpoint,
													 AMControl* globalZAxisFeedback,
													 AMControl* trajectoryStartControl,
													 const QString &name,
													 const QString &units,
													 QObject *parent,
													 const QString &description)
	: AM3DRotatedSystemPseudoMotorControl(axis, globalXAxisSetpoint, globalYAxisSetpoint, globalZAxisSetpoint, name, units, parent, description )
{
	trajectoryStartControl_ = trajectoryStartControl;
	globalXAxisFeedback_ = globalXAxisFeedback;
	globalYAxisFeedback_ = globalYAxisFeedback;
	globalZAxisFeedback_ = globalZAxisFeedback;

	addChildControl(globalXAxisFeedback_);
	addChildControl(globalYAxisFeedback_);
	addChildControl(globalZAxisFeedback_);
}

AMAction3 * SGMHexapodTransformedAxis::createMoveAction(double setpoint)
{
	AMListAction3* returnAction = 0;

	// In order to do this we're going to have to work with the trajectory
	// PVs, which will mean obtaining their current setpoints, getting the
	// move action(s) from the base class, tacking an action on to push to the
	// trajectory start PV, and then finally reverting the trajectory setpoints
	// back to their original values so as not to interfere with a trajectory that
	// a user was part way through setting.
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ && trajectoryStartControl_) {


		returnAction = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()),
															   QString("Moving %1").arg(name())),
										 AMListAction3::Sequential);


		// Save the current global setpoints
		QVector3D currentGlobalSetpoints(globalXAxis_->setpoint(), globalYAxis_->setpoint(), globalZAxis_->setpoint());

		// Add the actions for moving to the provided setpoint from the base class
		AMAction3* baseAction = AM3DRotatedSystemPseudoMotorControl::createMoveAction(setpoint);
		returnAction->addSubAction(baseAction);

		// Add an action to trigger the move.
		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(trajectoryStartControl_, 1));

//		// Add actions to return the setpoints to their original values
//		AMListAction3* restoreAction = new AMListAction3(new AMListActionInfo3(QString("Restoring %1").arg(name()),
//																			 QString("Restoring %1").arg(name())),
//													   AMListAction3::Parallel);
//		restoreAction->addSubAction(AMActionSupport::buildControlMoveAction(globalXAxis_, currentGlobalSetpoints.x()));
//		restoreAction->addSubAction(AMActionSupport::buildControlMoveAction(globalYAxis_, currentGlobalSetpoints.y()));
//		restoreAction->addSubAction(AMActionSupport::buildControlMoveAction(globalZAxis_, currentGlobalSetpoints.z()));
	}

	return returnAction;
}

void SGMHexapodTransformedAxis::updateConnected()
{
	setConnected(globalXAxis_->isConnected() &&
				 globalYAxis_->isConnected() &&
				 globalZAxis_->isConnected() &&
				 globalXAxisFeedback_->isConnected() &&
				 globalYAxisFeedback_->isConnected() &&
				 globalZAxisFeedback_->isConnected());

	updateMinimumValue();
	updateMaximumValue();
}

void SGMHexapodTransformedAxis::updateValue()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ &&
			globalXAxisFeedback_ && globalYAxisFeedback_ && globalZAxisFeedback_) {

		QVector3D globalValues(globalXAxisFeedback_->value(), globalYAxisFeedback_->value(), globalZAxisFeedback_->value());
		QVector3D primeValues = globalAxisToPrime(globalValues);

		double newValue = designatedAxisValue(primeValues);

		if(!withinTolerance(newValue)) {
			setValue(newValue);
		}
	}
}
#include <QDebug>
void SGMHexapodTransformedAxis::updateMoving()
{
	if(globalXAxis_ && globalYAxis_ && globalZAxis_ &&
			globalXAxisFeedback_ && globalYAxisFeedback_ && globalZAxisFeedback_) {
		bool isNowMoving = false;
		qDebug() << "\tSGMHexapodTransformedAxis::updateMoving() for " << name();
		qDebug() << "\t\tX feedback is moving: " << globalXAxisFeedback_->isMoving();
		qDebug() << "\t\tWe're affected by X: " << affectedByMotionsInX();
		qDebug() << "\t\tY feedback is moving: " << globalYAxisFeedback_->isMoving();
		qDebug() << "\t\tWe're affected by Y: " << affectedByMotionsInY();
		qDebug() << "\t\tZ feedback is moving: " << globalZAxisFeedback_->isMoving();
		qDebug() << "\t\tWe're affected by Z: " << affectedByMotionsInZ();
		isNowMoving |= (globalXAxisFeedback_->isMoving() && affectedByMotionsInX());
		isNowMoving |= (globalYAxisFeedback_->isMoving() && affectedByMotionsInY());
		isNowMoving |= (globalZAxisFeedback_->isMoving() && affectedByMotionsInZ());


		if(isNowMoving != isMoving()) {
			qDebug() << "\tSetting movement status to: " << isNowMoving;
			setIsMoving(isNowMoving);
		}
	}
}
