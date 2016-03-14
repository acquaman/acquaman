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


#ifndef SGMSIDEBAR_H
#define SGMSIDEBAR_H

#include <QWidget>

class QGroupBox;
class QVBoxLayout;
class QGridLayout;
class QRadioButton;
class QCheckBox;
class QToolButton;

class AMControlButton;

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/SGMBeamline.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"
#include "source/ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h"

class SGMSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit SGMSidebar(QWidget *parent = 0);

	virtual ~SGMSidebar();

protected:
	void showEvent(QShowEvent *);

protected slots:
	void onVisibleLightStatusChanged(const QString& status);
	void onVisibleLightClicked();
	void onCloseVacuumButtonClicked();
	void onBeamOnButtonClicked();
	void onBeamOnActionFinished();
	void onStopMotorsButtonClicked();
	void onStopMotorsActionFinished();

	/// Handles clicks from the button to run the actions
	void onRestoreBeamlineSettingsButtonClicked();
	/// Handles cleaning up after the restore actions
	void onRestoreBeamlineActionFinished();

	void onScanningResetButtonClicked();

	void onStripToolTimerTimeout();
	void onI0CheckBoxToggled(bool toggled);
	void onTEYCheckBoxToggled(bool toggled);
	void onTFYCheckBoxToggled(bool toggled);
	void onPDCheckBoxToggled(bool toggled);
	void onFPD1CheckBoxToggled(bool toggled);
	void onFPD2CheckBoxToggled(bool toggled);
	void onFPD3CheckBoxToggled(bool toggled);
	void onFPD4CheckBoxToggled(bool toggled);

	void onMinSpinBoxEditingFinished();
	void onMaxSpinBoxEditingFinished();
	void onAutoScaleCheckBoxToggled(bool toggled);
	void onLogCheckBoxToggled(bool toggled);

	void onBeamlineWarnings(const QString &newWarnings);

	void onBeamlineCriticalControlSetConnectedChanged(bool isConnected);
	void onBeamlineCriticalDetectorSetConnectedChanged(bool isConnected);

protected:
	QGroupBox *mainBox_;

	QVBoxLayout *mainLayout_;
	QGridLayout *gl_;

	AMExtendedControlEditor *readyLabel_;
	QToolButton *beamOnButton_;
	AMControlButton *beamOffCButton_;
	QToolButton *stopMotorsButton_;
	QToolButton *closeVacuumButton_;
	QToolButton *visibleLightButton_;
	AMExtendedControlEditor *energyNC_;
	AMControlButton *trackUndulatorCButton_;
	AMControlButton *trackGratingCButton_;
	AMControlButton *trackExitSlitCButton_;
	AMExtendedControlEditor *gratingNC_;
	AMExtendedControlEditor *exitSlitNC_;
	AMExtendedControlEditor *scanningLabel_;
	QToolButton *scanningResetButton_;
	QHBoxLayout *warningAndPlotHL_;
	QLabel *beamlineWarningsLabel_;
	/// Runs the actions to clean up after fast scanning problems (sets to default values)
	QPushButton *restoreBeamlineSettingsButton_;

	QCheckBox *i0CheckBox_, *teyCheckBox_, *tfyCheckBox_, *pdCheckBox_, *fpd1CheckBox_, *fpd2CheckBox_, *fpd3CheckBox_, *fpd4CheckBox_;
	QCheckBox *autoScaleCheckBox_, *logCheckBox_;
	QLabel *minLabel_, *maxLabel_;
	QDoubleSpinBox *minSpinBox_, *maxSpinBox_;

	QVBoxLayout *plotLayout_;

	int plotLeftAxisMinimum_;
	int plotLeftAxisMaximum_;

	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotRealtimeModel *i0Model_, *teyModel_, *tfyModel_, *pdModel_, *fpd1Model_, *fpd2Model_, *fpd3Model_, *fpd4Model_;
	MPlotSeriesBasic *i0Series_, *teySeries_, *tfySeries_, *pdSeries_, *fpd1Series_, *fpd2Series_, *fpd3Series_, *fpd4Series_;

	QTimer *stripToolTimer_;
	int stripToolCounter_;
	MPlotAxisScale* stripToolSpecialAxisScale_;

	AMAction3 *beamOnAction_;
	AMAction3 *stopMotorsAction_;
	AMAction3 *restoreBeamlineAction_;

	QLabel *controlsConnectedLabel_;
	QLabel *detectorsConnectedLabel_;
};

#endif // SGMSIDEBAR_H
