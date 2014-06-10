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


#ifndef SGMFASTSCANSETTINGSVIEW_H
#define SGMFASTSCANSETTINGSVIEW_H

#include <QGroupBox>
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;

class SGMFastScanSettingsView : public QGroupBox
{
Q_OBJECT

public:
 	virtual ~SGMFastScanSettingsView();
	SGMFastScanSettingsView(SGMFastScanSettings *fastScanSettings, QWidget *parent = 0);

public slots:
	void setFastScanSettings(SGMFastScanSettings *fastScanSettings);

protected slots:
	void onRunSecondsEditingFinished();
	void onMotorSettingsEditingFinished();
	void onBaseLineEditingFinished();
	void onUndulatorVelocityEditingFinished();

protected:
	SGMFastScanSettings *fastScanSettings_;

	QDoubleSpinBox *runSecondsSpinBox_;
	QSpinBox *motorSettingsSpinBox_;
	QSpinBox *baseLineSpinBox_;
	QSpinBox *undulatorVelocitySpinBox_;
};

#endif // SGMFASTSCANSETTINGSVIEW_H
