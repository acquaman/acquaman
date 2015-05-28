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

#ifndef AM4DMOTORGROUPVIEW_H
#define AM4DMOTORGROUPVIEW_H

#include "ui/AMMotorGroupView.h"

class QGridLayout;
class QPushButton;
class QCheckBox;

class AMExtendedControlEditor;

class AM4DMotorGroupObjectView : public AMMotorGroupObjectView
{
Q_OBJECT
public:
	AM4DMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

	virtual ~AM4DMotorGroupObjectView();

	void enableMoveInFocus(bool enable);

protected slots:
	/// Slot that handles the fourth motor setpoint.
	virtual void onFourthControlSetpoint();

	/// Slot that handles the fourth motor rotate (CW)
	virtual void onRotateCWClicked();
	/// Slot that handles the fourth motor rotate (CCW)
	virtual void onRotateCCWClicked();

	/// Slot that handles the motor moving control (disable the fourth motor when the others is moving)
	virtual void onMovingChanged();

	/// Slot that handles the setInFocus button clicked signal
	void onSetInFocusButtonClicked();
	/// Slot that handles the moveInFocus checkbox toggled signal
	void onMoveInFocusCheckBoxToggled(bool moveInFocus);

protected:
	/// we have a moveInFocus feature addedd for this type of Motor, by default we want to disable it
	bool moveInFocusEnabled_;

	/// The buttons to control the fourth motor (rotate)
	QToolButton *rotateCW_;
	QToolButton *rotateCCW_;

	/// the components to control the move in Focus feature, which is disabled by default
	QPushButton *setInFocusButton_;
	QCheckBox *moveInFocusFocusCheckBox_;
};

#endif // AM4DMOTORGROUPVIEW_H
