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


#ifndef AMSAMPLEPLATEPRE2013MOVEACTIONEDITOR_H
#define AMSAMPLEPLATEPRE2013MOVEACTIONEDITOR_H

#include <QFrame>
#include "actions3/actions/AMSamplePlatePre2013MoveActionInfo.h"

class QComboBox;
class QLabel;
class QCheckBox;

#define AMSAMPLEPLATEMOVEACTIONEDITOR_ATTEMPTED_TO_LOAD_BAD_PLATE 327201

/// This widget provides an in-place editor for AMControlMoveActionInfo.
class AMSamplePlatePre2013MoveActionEditor : public QFrame
{
Q_OBJECT

public:
	/// Constructor
	Q_INVOKABLE AMSamplePlatePre2013MoveActionEditor(AMSamplePlatePre2013MoveActionInfo *info, QWidget *parent = 0);

	virtual ~AMSamplePlatePre2013MoveActionEditor();

protected slots:
	/// Called when the sample position combo box is activated to choose a different sample on the plate.
	void onSamplePositionSelectorBoxCurrentIndexChanged(int index);
	/// Called when the sample plate combo box is activated to choose a different sample plate altogether
	void onSamplePlateSeletectorBoxCurrentIndexChanged(int index);
	/// Called when the change sample plate check is toggled
	void onChangeSamplePlateCheckBoxToggled(bool canChangePlate);


protected:
	AMSamplePlatePre2013MoveActionInfo *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* samplePositionSelectorBox_;
	QComboBox* samplePlateSelectorBox_;
	QLabel* unitsLabel_, *toLabel_;
	QCheckBox* changeSamplePlateCheckBox_;

	/// Helper function to populate the available sample plates, and update the widgets to reflect the current info_.
	void populateSamplePlates();

	/// Helper function to populate the available sample positions, and update the widgets to reflect the current info_.
	void populateSamplePositions();
};

#endif // AMSAMPLEPLATEPRE2013MOVEACTIONEDITOR_H
