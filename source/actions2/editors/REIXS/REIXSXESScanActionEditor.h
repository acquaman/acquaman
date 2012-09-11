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


#ifndef REIXSXESSCANACTIONEDITOR_H
#define REIXSXESSCANACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/REIXS/REIXSXESScanActionInfo.h"

class QDoubleSpinBox;
class QComboBox;

class REIXSXESScanActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be invokable, and accept a REIXSXESScanActionInfo as argument
	Q_INVOKABLE REIXSXESScanActionEditor(REIXSXESScanActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When the detector energy setpoint spin box is changed
	void onEnergyEditingFinished();
//	/// When the focus offset spin box is changed
//	void onFocusOffsetEditingFinished();
	/// When the tilt offset spin box is changed
	void onTiltOffsetEditingFinished();
	/// Called when the grating combo box is changed; sets the grating in the scan config.
	void onGratingChoiceActivated(int index);
	/// When the "More..." button is clicked, opens up a REIXSXESScanConfigurationView in a dialog window.
	void onMoreDetailsButtonClicked();

	/// Updates the elements when the scan configuration is changed from outside.
	void onScanConfigurationChanged();


protected:
	QDoubleSpinBox* energy_, /* *focusOffset_,*/ *tiltOffset_;
	QComboBox* grating_;
	REIXSXESScanActionInfo* info_;

};


#endif // REIXSXESSCANACTIONEDITOR_H
