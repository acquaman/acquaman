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

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"

class SGMFastScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

signals:

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();

	void onScanNameEditChanged(const QString &scanName);

	void onParametersStartPositionChanged();
	void onParametersMiddlePositionChanged();
	void onParametersEndPositionChanged();
	void onFastScanSettingsChanged();

	void onStartPositionCopyChanged();
	void onMiddlePositionCopyChanged();
	void onEndPositionCopyChanged();
	void onFastScanSettingsCopyChanged();

protected:
	SGMFastScanConfiguration *cfg_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionView *startPositionView_;
	SGMEnergyPositionView *middlePositionView_;
	SGMEnergyPositionView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

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

	QLabel *scanNameLabel_;
	QLineEdit *scanNameEdit_;

	QFormLayout *fl_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
