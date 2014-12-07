/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H
#define SGMSAMPLEMANIPULATORMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

class QGridLayout;
class QPushButton;
class QCheckBox;

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

	void onSetInFocusButtonClicked();
	void onMoveInFocusCheckBoxToggled(bool moveInFocus);

protected:
	QToolButton *rotateCW_;
	QToolButton *rotateCCW_;

	QPushButton *setInFocusButton_;
	QCheckBox *moveInFocusFocusCheckBox_;

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
