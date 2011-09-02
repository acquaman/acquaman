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


#ifndef SGMSIDEBAR_H
#define SGMSIDEBAR_H

#include <QWidget>

class QGroupBox;
class QVBoxLayout;
class QGridLayout;
class QRadioButton;

#include "ui/AMControlEditor.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/AMBeamlineParallelActionsList.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

class SGMSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit SGMSidebar(QWidget *parent = 0);

	~SGMSidebar();

signals:

public slots:
	// debugging: void testingBoundsChanged();

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

	void onDetectorSignalSourceChanged(SGMBeamline::sgmDetectorSignalSource newSource);
	void onDetectorButtonsClicked(int buttonIndex);

	void onCurrentEndstationChanged(SGMBeamline::sgmEndstation newEndstation);
	void onEndstationButtonsClicked(int buttonIndex);

	void onScanningResetButtonClicked();

	void onStripToolTimerTimeout();

	void onBeamlineWarnings(const QString &newWarnings);

	void onHVOnClicked();
	void onHVOffClicked();
	void onHVOnSucceeded();
	void onHVOffSucceeded();

protected:
	QGroupBox *mainBox_;

	QVBoxLayout *mainLayout_;
	QGridLayout *gl_;

	AMControlEditor *readyLabel_;
	QToolButton *beamOnButton_;
	AMControlButton *beamOffCButton_;
	QToolButton *stopMotorsButton_;
	QToolButton *closeVacuumButton_;
	QToolButton *visibleLightButton_;
	AMControlEditor *energyNC_;
	AMControlButton *trackUndulatorCButton_;
	AMControlButton *trackGratingCButton_;
	AMControlButton *trackExitSlitCButton_;
	AMControlEditor *gratingNC_;
	AMControlEditor *entranceSlitNC_;
	AMControlEditor *exitSlitNC_;
	AMControlEditor *scanningLabel_;
	QToolButton *scanningResetButton_;
	QButtonGroup *detectorSignalSources_;
	QRadioButton *picoammeterButton_;
	QRadioButton *scalerButton_;
	QButtonGroup *endstationsAvailable_;
	QRadioButton *scientaButton_;
	QRadioButton *ssaButton_;
	QHBoxLayout *warningAndPlotHL_;
	QLabel *beamlineWarningsLabel_;

	QPushButton *hvOnButton_;
	QPushButton *hvOffButton_;

	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotRealtimeModel *i0Model_, *teyModel_, *tfyModel_, *pdModel_;
	MPlotSeriesBasic *i0Series_, *teySeries_, *tfySeries_, *pdSeries_;

	QTimer *stripToolTimer_;
	int stripToolCounter_;
	MPlotAxisScale* stripToolSpecialAxisScale_;

	AMBeamlineListAction *beamOnAction_;
	AMBeamlineListAction *stopMotorsAction_;


};

#endif // SGMSIDEBAR_H
