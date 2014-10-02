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


#ifndef REIXSXESImageInterpolationABEDITOR_H
#define REIXSXESImageInterpolationABEDITOR_H

#include <QWidget>

class QSpinBox;
class QCheckBox;
class QPushButton;
class QSlider;
class QAction;
class QToolButton;
class QDoubleSpinBox;
class QComboBox;


class MPlotWidget;
class MPlot;
class MPlotImageBasic;
class MPlotRectangle;
class MPlotSeriesBasic;
class MPlotColorMap;
class MPlotPoint;

class AMChooseScanDialog;
class REIXSXESImageInterpolationAB;

#include "MPlot/MPlotSeriesData.h"

/// Helper class for REIXSXESImageInterpolationABEditor: Exposes REIXSXESImageInterpolationAB's shift data as MPlotAbstractSeriesData
class REIXSXESImageInterpolationABEditorShiftModel : public QObject, public MPlotAbstractSeriesData {
	Q_OBJECT
public:
	/// Constructor: exposes the shiftValues() in \c analysisBlock (must be valid).
	REIXSXESImageInterpolationABEditorShiftModel(REIXSXESImageInterpolationAB* analysisBlock, QObject* parent = 0);

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
	REIXSXESImageInterpolationAB* analysisBlock_;
	/// Adds this offset to the x-values of the shift data, so that it can be shown at any point on the plot. Initialized to analysisBlock_->size(0)/2.
	int displayXOffset_;

};



/// Helper class for REIXSXESImageInterpolationABEditor: Exposes REIXSXESImageInterpolationAB's shift data as MPlotAbstractSeriesData
class REIXSXESImageInterpolationABEditorEllipticalMask : public QObject, public MPlotAbstractSeriesData {
	Q_OBJECT
public:
	/// Constructor: exposes the shiftValues() in \c analysisBlock (must be valid).
	REIXSXESImageInterpolationABEditorEllipticalMask(REIXSXESImageInterpolationAB* analysisBlock, QObject* parent = 0);

	virtual qreal x(unsigned index) const;
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;
	virtual qreal y(unsigned index) const;
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;
	virtual int count() const;
	//call when range or roundness changes: call emitDataChanged()
	void rangeValuesChanged();
	
public slots:

protected slots:
	/*
	/// Called when the size of the analysis block changes: reset displayXOffset_.
	void onOutputSizeChanged();
	/// Called when the shift values change: calls emitDataChanged().
	void onShiftValuesChanged();
	*/


protected:
	REIXSXESImageInterpolationAB* analysisBlock_;
	/*
	/// Adds this offset to the x-values of the shift data, so that it can be shown at any point on the plot. Initialized to analysisBlock_->size(0)/2.
	int displayXOffset_;
	*/
};






/// Provides an editor widget for REIXSXESImageInterpolationAB, showing the raw image, the shift curve, and options to control the summing range and correlation settings.
class REIXSXESImageInterpolationABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Construct an editor for the analysis block \c analysisBlock. (Must be valid)
	explicit REIXSXESImageInterpolationABEditor(REIXSXESImageInterpolationAB* analysisBlock, QWidget *parent = 0);

	/// Destructor
	~REIXSXESImageInterpolationABEditor();

	/// Enum describing the options for smoothing the auto-correlated shift curve.
	enum smoothBoxType { None, Poly, Median, Average };


signals:


public slots:

	/// signals from GUI objects: range minimum control
	void onRangeMinYControlChanged(int);
	/// signals from GUI objects: range maximum control
	void onRangeMaxYControlChanged(int);
	/// signals from GUI objects: range minimum control
	void onRangeMinXControlChanged(int);
	/// signals from GUI objects: range maximum control
	void onRangeMaxXControlChanged(int);
	
	/// signals from GUI objects: mask roundness control
	void onRangeRoundControlChanged(double);
	
	
	/// Called when the correlation settings are changed
	void onCorrelationCenterBoxChanged(int);
	void onCorrelationPointsBoxChanged(int);
	void onCSmoothBoxChanged();
	void onCSmoothModeChanged();


	// The "Apply to other scans" button applies this shift curve to many scans at once.
	/// Called to apply same shift curve to many XES scans. Shows the dialog to choose which scans.
	void onApplyToOtherScansMenuClicked();
	/// When a user chooses which scans to apply the current shift to.
	void onApplyToOtherScansChosen();
	/// When a user pushes the "manual shift entry" button.
	void onManualShiftEntryButtonClicked();

	/// signals from analysis block: if the block's input data source changes (to either null, or one with a different size.)
	void onAnalysisBlockInputDataSourcesChanged();

	/*
	 Unused; REIXSXESImageInterpolationABEditorShiftModel handles this.
		/// Called when the shift values change, so we can update our line plot
		void onShiftValuesChanged();
	*/

protected:

	REIXSXESImageInterpolationAB* analysisBlock_;

	// GUI elements:
	QSpinBox* rangeMinYControl_, *rangeMaxYControl_, *rangeMinXControl_, *rangeMaxXControl_;
	QSpinBox* correlationCenterBox_, *correlationPointsBox_, *smoothModeBox_;
	QPushButton* correlateNowButton_;
	QComboBox* correlationSmoothingBox_;
	QCheckBox* liveCorrelationCheckBox_;
	QDoubleSpinBox* energyCalibrationOffsetBox_, *tiltCalibrationOffsetBox_, *rangeRoundControl_;

	QSlider* shiftDisplayOffsetSlider_;

	QToolButton* applyToOtherScansButton_;
	QToolButton* manualShiftEntryButton_;


	// plot widget to show what region is summed
	MPlotWidget* plotWidget_;
	MPlot* plot_;
	MPlotColorMap* colorMap_;
	MPlotImageBasic* image_;
	MPlotRectangle* rangeRectangleY1_, *rangeRectangleY2_, *rangeRectangleX1_, *rangeRectangleX2_;
	
	MPlotSeriesBasic* shiftSeries_;
	MPlotPoint* corrRegionLeft_, * corrRegionRight_;
	REIXSXESImageInterpolationABEditorShiftModel* shiftData_;
	
	MPlotSeriesBasic* ellipseSeries_;
	REIXSXESImageInterpolationABEditorEllipticalMask* ellipseData_;
	

	/// Dialog to ask the user for a set of scans (to apply the same shift curve to many at once)
	AMChooseScanDialog* chooseScanDialog_;
	/// A checkable menu action that indicates "batch apply" should apply correlation settings to all scans.
	QAction* batchApplyCorrelationSettings_;
	/// A checkable menu action that indicates "batch apply" should apply the shift curve to all scans.
	QAction* batchApplyShiftCurve_;
	/// A checkable menu action that indicates "batch apply" should apply the sum range (min, max) to all scans.
	QAction* batchApplySumRange_;
	/// A checkable menu action that indicates "batch apply" shouuld apply the calibration offsets (energy, tilt) to all scans.
	QAction* batchApplyCalibrationOffsets_;


	/// called to position and show/hide the range rectangle, as appropriate.
	void placeRangeRectangle();
	

};




#endif // REIXSXESImageInterpolationABEDITOR_H
