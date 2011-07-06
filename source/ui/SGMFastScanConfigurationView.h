/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

class QGridLayout;
class QComboBox;
class QLabel;
class QFileDialog;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QFormLayout;

class AMTopFrame;

#include <algorithm>

#include "AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

signals:

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();

protected:
	SGMFastScanConfiguration *cfg_;

	AMTopFrame *topFrame_;

	QComboBox *presetsComboBox_;
	QLabel *elementLabel_;
	QLabel *runTimeLabel_;
	QLabel *startEnergyLabel_;
	QLabel *energyMidpointLabel_;
	QLabel *endEnergyLabel_;
	QLabel *motorSettingsLabel_;
	QLabel *baseLineLabel_;
	QLabel *undulatorStartStepLabel_;
	QLabel *undulatorRelativeStepLabel_;
	QLabel *undulatorVelocityLabel_;
	QLabel *exitSlitDistanceLabel_;
	QLabel *warningsLabel_;

	QLineEdit *elementEdit_;
	QDoubleSpinBox *runTimeDSB_;
	QDoubleSpinBox *startEnergyDSB_;
	QDoubleSpinBox *energyMidpointDSB_;
	QDoubleSpinBox *endEnergyDSB_;
	QSpinBox *motorSettingsSB_;
	QSpinBox *baseLineSB_;

	QSpinBox *undulatorStartStepSB_;
	QSpinBox *undulatorRelativeStepSB_;
	QSpinBox *undulatorVelocitySB_;
	QDoubleSpinBox *exitSlitDistanceDSB_;

	QGridLayout *gl_;
	QFormLayout *fl_;
	QFormLayout *fl2_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
