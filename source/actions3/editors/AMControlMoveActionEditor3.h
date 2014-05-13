/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMCONTROLMOVEACTIONEDITOR3_H
#define AMCONTROLMOVEACTIONEDITOR3_H

#include <QFrame>
#include "actions3/actions/AMControlMoveActionInfo3.h"

class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QCheckBox;

/// This widget provides an in-place editor for AMControlMoveActionInfo.
class AMControlMoveActionEditor3 : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be invokable, and accept a AMControlMoveActionInfo as argument
 	virtual ~AMControlMoveActionEditor3();
	Q_INVOKABLE AMControlMoveActionEditor3(AMControlMoveActionInfo3 *info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the combo box is activated to choose a different control. Sets the min/max range of the setpointBox_, and sets the info_ to use that control.
	void onControlSelectorBoxCurrentIndexChanged(int index);
	/// Called when the user is done editing the setpointBox_ target value.
	void onSetpointBoxEditingFinished();
	/// Called when the "relative move" checkbox is toggled
	void onRelativeCheckBoxToggled(bool isRelative);

protected:
	AMControlMoveActionInfo3 *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* controlSelectorBox_;
	QDoubleSpinBox* setpointBox_;
	QLabel* unitsLabel_, *toLabel_;
	QCheckBox* relativeCheckBox_;

	/// Helper function to populate the available controls in the controlSelectorBox_, and update the widgets to reflect the current info_.
	void populateControls();
};

#endif // REIXSCONTROLMOVEACTIONEDITOR_H
