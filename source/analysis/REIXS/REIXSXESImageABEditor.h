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


#ifndef REIXSXESIMAGEABEDITOR_H
#define REIXSXESIMAGEABEDITOR_H

#include <QWidget>

class QSpinBox;
class QCheckBox;
class QPushButton;
class QSlider;


class MPlotWidget;
class MPlot;
class MPlotImageBasic;
class MPlotRectangle;
class MPlotSeriesBasic;
class MPlotColorMap;
class MPlotPoint;

class REIXSXESImageAB;

#include "MPlot/MPlotSeriesData.h"

/// Helper class for REIXSXESImageABEditor: Exposes REIXSXESImageAB's shift data as MPlotAbstractSeriesData
class REIXSXESImageABEditorShiftModel : public QObject, public MPlotAbstractSeriesData {
	Q_OBJECT
public:
	/// Constructor: exposes the shiftValues() in \c analysisBlock (must be valid).
	REIXSXESImageABEditorShiftModel(REIXSXESImageAB* analysisBlock, QObject* parent = 0);

	virtual qreal x(unsigned index) const;
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;
	virtual qreal y(unsigned index) const;
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;
	virtual int count() const;

public slots:
	/// Sets the amount that the shift curve is offset along the X axis of the image plot (so users can match it up with peaks).
	void setDisplayXOffset(int offset);

protected slots:
	/// Called when the size of the analysis block changes: reset displayXOffset_.
	void onOutputSizeChanged();
	/// Called when the shift values change: calls emitDataChanged().
	void onShiftValuesChanged();

protected:
	REIXSXESImageAB* analysisBlock_;
	/// Adds this offset to the x-values of the shift data, so that it can be shown at any point on the plot. Initialized to analysisBlock_->size(0)/2.
	int displayXOffset_;

};


/// Provides an editor widget for REIXSXESImageAB, showing the raw image, the shift curve, and options to control the summing range and correlation settings.
class REIXSXESImageABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Construct an editor for the analysis block \c analysisBlock. (Must be valid)
	explicit REIXSXESImageABEditor(REIXSXESImageAB* analysisBlock, QWidget *parent = 0);

	/// Destructor
	~REIXSXESImageABEditor();


signals:


public slots:

	/// signals from GUI objects: range minimum control
	void onRangeMinControlChanged(int);
	/// signals from GUI objects: range maximum control
	void onRangeMaxControlChanged(int);
	/// Called when the correlation settings are changed
	void onCorrelationCenterBoxChanged(int);
	void onCorrelationPointsBoxChanged(int);


	/// signals from analysis block: if the block's input data source changes (to either null, or one with a different size.)
	void onAnalysisBlockInputDataSourcesChanged();

	// Unused; REIXSXESImageABEditorShiftModel handles this.
	//	/// Called when the shift values change, so we can update our line plot
	//	void onShiftValuesChanged();



protected:

	REIXSXESImageAB* analysisBlock_;

	// GUI elements:
	QSpinBox* rangeMinControl_, *rangeMaxControl_;
	QSpinBox* correlationCenterBox_, *correlationPointsBox_;
	QPushButton* correlateNowButton_;
	QCheckBox* liveCorrelationCheckBox_;
	QSlider* shiftDisplayOffsetSlider_;

	// plot widget to show what region is summed
	MPlotWidget* plotWidget_;
	MPlot* plot_;
	MPlotColorMap* colorMap_;
	MPlotImageBasic* image_;
	MPlotRectangle* rangeRectangle1_, *rangeRectangle2_;
	MPlotSeriesBasic* shiftSeries_;
	MPlotPoint* corrRegionLeft_, * corrRegionRight_;
	REIXSXESImageABEditorShiftModel* shiftData_;


	/// called to position and show/hide the range rectangle, as appropriate.
	void placeRangeRectangle();

};




#endif // REIXSXESIMAGEABEDITOR_H
