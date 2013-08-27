#ifndef SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H
#define SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

class QGridLayout;

class SGMSampleManipulatorMotorGroupObjectView : public AMMotorGroupObjectView
{
Q_OBJECT
public:
	SGMSampleManipulatorMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

protected slots:
	void onRotateCWClicked();
	void onRotateCCWClicked();
	void onFourthControlSetpoint();

	virtual void onMovingChanged();

protected:
	QToolButton *rotateCW_;
	QToolButton *rotateCCW_;
};

class SGMSampleManipulatorMotorGroupView : public AMMotorGroupView
{
Q_OBJECT
public:
	SGMSampleManipulatorMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent = 0);
};

#endif // SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H
