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


#ifndef REIXSRSXSMCPDETECTORVIEW_H
#define REIXSRSXSMCPDETECTORVIEW_H

#include <QWidget>
#include "ui/beamline/AMControlEditor.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotImage.h"

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QRadioButton>

class QDialog;
class REIXSRSXSMCPDataSource;
class AMReadOnlyPVControl;
class AMSinglePVControl;
class AMReadOnlyPVControl;
class MPlotDataPositionTool;
struct MPlotRectangle;

class REIXSRSXSMCPDetectorView : public QWidget
{
	Q_OBJECT
public:
	REIXSRSXSMCPDetectorView(QWidget *parent = 0);
	virtual ~REIXSRSXSMCPDetectorView();

protected:
	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotImageBasic* image_;
	MPlotDataPositionTool *positionTool_;
	REIXSRSXSMCPDataSource *dataSource_;
	AMReadOnlyPVControl *imageControl_;
	AMSinglePVControl *topLeftX1_, *topLeftY1_, *bottomRightX1_, *bottomRightY1_;
	AMSinglePVControl *topLeftX2_, *topLeftY2_, *bottomRightX2_, *bottomRightY2_;
	AMReadOnlyPVControl *getTopLeftX1_, *getTopLeftY1_, *getBottomRightX1_, *getBottomRightY1_;
	AMReadOnlyPVControl *getTopLeftX2_, *getTopLeftY2_, *getBottomRightX2_, *getBottomRightY2_;
	QPushButton *adjustColorMapButton_, *updateButton_, *acquireButton_;


	QDialog* colorMapEditor_;
	MPlotRectangle* rangeRectangle1_, *rangeRectangle2_;
	AMSinglePVControl *waysToGetBuffer_, *downloadMemory_, *startAccumulation_;
	AMReadOnlyPVControl *ROI1Counts_,*ROI2Counts_, *totalCounts_;
	QLabel *ROI1CountsLabel_, *ROI2CountsLabel_, *totalCountsLabel_;
	QRadioButton *ROI1Select_, *ROI2Select_;


protected slots:
	/// not actually used
	void onDataPositionChanged(QPointF);
	/// triggered when a region is drag-selected
	void onSelectedDataRectChanged(QRectF);
	/// When button to trigger image download is clicked
	void onUpdateButtonClicked();
	/// When button to trigger image acquistiton is clicked
	void onAcquireButtonClicked();
	/// When the button to adjust the plot image's color map is clicked
	void onAdjustColorMapButtonClicked();
	/// When the color map inside the dialog is changed
	void onColorMapChanged(const MPlotColorMap& map);
	/// Draws currently set regions from 'get' PVs
	void drawRegions();
	/// Updates the displayed counts when PVs update
	void updateCounts();



};





#endif // REIXSRSXSMCPDETECTORVIEW_H
