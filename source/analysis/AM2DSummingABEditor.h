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


#ifndef AM2DSUMMINGABEDITOR_H
#define AM2DSUMMINGABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

class MPlotWidget;
class MPlot;
class MPlotImageBasic;
class MPlotRectangle;

class AM2DSummingAB;

class AM2DSummingABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Construct an editor for the analysis block \c analysisBlock. (Must be valid)
 	virtual ~AM2DSummingABEditor();
	AM2DSummingABEditor(AM2DSummingAB* analysisBlock, QWidget *parent = 0);

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

protected:
	/// The analysis block instance we provide editing capabilities for
	AM2DSummingAB* analysisBlock_;

	/// Gui elements
	QComboBox* axisSelector_;
	QSpinBox* rangeMinControl_, *rangeMaxControl_;

	/// plot widget to show what region is summed
	MPlotWidget* plotWidget_;
	MPlot* plot_;
	MPlotImageBasic* image_;
	MPlotRectangle* rangeRectangle1_, *rangeRectangle2_;

	/// called to position and show/hide the range rectangle, as appropriate.
	void placeRangeRectangle();


	/// The combo box that contains all of the names.
	QComboBox *names_;
};

#endif // AM2DSUMMINGABEDITOR_H
