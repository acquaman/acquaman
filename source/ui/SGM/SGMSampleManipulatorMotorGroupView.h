#ifndef SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H
#define SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

class QGridLayout;

class AMExtendedControlEditor;

class SGMSampleManipulatorMotorGroupObjectView : public AMMotorGroupObjectView
{
Q_OBJECT
public:
	SGMSampleManipulatorMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

	virtual ~SGMSampleManipulatorMotorGroupObjectView();

protected slots:
	/// Slot that handles going up.
	virtual void onUpClicked();
	/// Slot that handles going down.
	virtual void onDownClicked();
	/// Slot that handles going left.
	virtual void onLeftClicked();
	/// Slot that handles going right.
	virtual void onRightClicked();
	/// Slot that handles going in.
	virtual void onInClicked();
	/// Slot that handles going out.
	virtual void onOutClicked();

	/// Slot that handles the first motor setpoint.
	virtual void onFirstControlSetpoint();
	/// Slot that handles the second motor setpoint.
	virtual void onSecondControlSetpoint();
	/// Slot that handles the third motor setpoint.
	virtual void onThirdControlSetpoint();

	virtual void onRotateCWClicked();
	virtual void onRotateCCWClicked();

	virtual void onMovingChanged();

protected:
	QToolButton *rotateCW_;
	QToolButton *rotateCCW_;

	/// The tolerance below which we don't care that the motor moved so we don't get jittering updates
	double controlTolerane_;

	QList<AMExtendedControlEditor*> controlEditors_;
};

class SGMSampleManipulatorMotorGroupView : public AMMotorGroupView
{
Q_OBJECT
public:
	SGMSampleManipulatorMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent = 0);

	virtual ~SGMSampleManipulatorMotorGroupView();
};

#endif // SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H
