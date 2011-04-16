/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>

#include "AMControlEditor.h"
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

signals:

public slots:

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

	void onStripToolTimerTimeout();

protected:
	QGroupBox *mainBox_;

	QVBoxLayout *mainLayout_;
	QGridLayout *gl_;

	AMControlEditor *readyLabel_;
	//AMBeamlineActionButton *beamOnBALButton_;
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
	QLabel *beamlineWarningsLabel_;

	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotRealtimeModel *i0Model_, *teyModel_, *tfyModel_, *pdModel_;
	MPlotSeriesBasic *i0Series_, *teySeries_, *tfySeries_, *pdSeries_;

	QTimer *stripToolTimer_;
	int stripToolCounter_;

	//AMBeamlineParallelActionsList *al;
	AMBeamlineListAction *beamOnAction_;
	AMBeamlineListAction *stopMotorsAction_;
};

#endif // SGMSIDEBAR_H
