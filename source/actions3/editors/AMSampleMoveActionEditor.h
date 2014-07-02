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


#ifndef AMSAMPLEMOVEACTIONEDITOR_H
#define AMSAMPLEMOVEACTIONEDITOR_H

#include <QFrame>

#include "actions3/actions/AMSampleMoveActionInfo.h"

class QComboBox;
class QLabel;

class AMSampleMoveActionEditor : public QFrame
{
Q_OBJECT
public:
	Q_INVOKABLE AMSampleMoveActionEditor(AMSampleMoveActionInfo *info, QWidget *parent = 0);

	virtual ~AMSampleMoveActionEditor();

protected slots:
	/// Called when the sample plate combo box is activated to choose a different sample plate altogether
	void onSampleSeletectorBoxCurrentIndexChanged(int index);

protected:
	AMSampleMoveActionInfo *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* sampleSelectorBox_;
	QLabel* unitsLabel_, *toLabel_;

	/// Helper function to populate the available sample positions, and update the widgets to reflect the current info_.
	void populateSamples();
};

#endif // AMSAMPLEMOVEACTIONEDITOR_H
