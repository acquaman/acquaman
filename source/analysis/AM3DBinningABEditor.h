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


#ifndef AM3DBINNINGABEDITOR_H
#define AM3DBINNINGABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

class MPlotWidget;
class MPlot;
class MPlotSeriesBasic;
class MPlotVectorSeriesData;
class MPlotRectangle;

class AM3DBinningAB;

class AM3DBinningABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Construct an editor for the analysis block \c analysisBlock.
 	virtual ~AM3DBinningABEditor();
	explicit AM3DBinningABEditor(AM3DBinningAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:
	/// signals from GUI objects: axis selector
	void onSumAxisControlChanged(int);
	/// signals from GUI objects: range minimum control
	void onRangeMinControlChanged(int);
	/// signals from GUI objects: range maximum control
	void onRangeMaxControlChanged(int);

	/// signals from analysis block: if the block's input data source changes (to either null, or one with a different size.)
	void onAnalysisBlockInputDataSourcesChanged();

protected slots:
	/// Handles setting the name for the analysis block based on the new choice of input data source.
	void onNameChoiceChanged(int index);
	/// Helper slot.  Populates the names combo box.
	void populateComboBox();
	/// Helper slot.  Handles updating the series data.
	void updateSeriesData();

protected:
	/// called to position and show/hide the range rectangle, as appropriate.
	void placeRangeRectangle();

	/// The analysis block instance we provide editing capabilities for
	AM3DBinningAB* analysisBlock_;

	/// Gui elements
	QComboBox* axisSelector_;
	QSpinBox* rangeMinControl_, *rangeMaxControl_;
	/// The combo box that contains all of the names.
	QComboBox *names_;

	/// plot widget to show what region is summed
	MPlotWidget *plotWidget_;
	MPlot *plot_;
	MPlotSeriesBasic *series_;
	MPlotVectorSeriesData *seriesData_;
	MPlotRectangle *rangeRectangle1_, *rangeRectangle2_;
};

#endif // AM3DBINNINGABEDITOR_H
