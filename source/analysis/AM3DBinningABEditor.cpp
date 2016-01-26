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


#include "AM3DBinningABEditor.h"

#include "analysis/AM3DBinningAB.h"

#include <QFormLayout>
#include <QBoxLayout>
#include <QLabel>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotSeriesData.h"

 AM3DBinningABEditor::~AM3DBinningABEditor(){}
AM3DBinningABEditor::AM3DBinningABEditor(AM3DBinningAB *analysisBlock, QWidget *parent)
	: QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	names_ = new QComboBox;
	populateComboBox();

	connect(names_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNameChoiceChanged(int)));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));

	axisSelector_ = new QComboBox();
	axisSelector_->addItem("Horizontal");
	axisSelector_->addItem("Vertical");
	axisSelector_->addItem("Depth");

	rangeMinControl_ = new QSpinBox();
	rangeMinControl_->setSingleStep(1);
	rangeMinControl_->setMinimum(0);
	rangeMaxControl_ = new QSpinBox();
	rangeMaxControl_->setSingleStep(1);
	rangeMaxControl_->setMinimum(0);

	plot_ = new MPlot();
	plotWidget_ = new MPlotWidget();
	plotWidget_->setPlot(plot_);

	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->axisRight()->setTicks(0);
	plot_->axisBottom()->setTicks(4);
	plot_->axisLeft()->showGrid(false);
	plot_->axisBottom()->showAxisName(false);
	plot_->axisLeft()->showAxisName(false);
	plot_->axisScaleBottom()->setPadding(0);
	plot_->axisScaleLeft()->setPadding(0);
	seriesData_ = new MPlotVectorSeriesData();
	series_ = new MPlotSeriesBasic();
	series_->setMarker(MPlotMarkerShape::None);
	series_->setModel(seriesData_, true);
	plot_->addItem(series_);

	QColor white(Qt::white);
	QPen pen(white, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	white.setAlphaF(0.6);
	rangeRectangle1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangle1_->setSelectable(false);
	rangeRectangle1_->setIgnoreWhenAutoScaling(true);
	rangeRectangle1_->setZValue(3000);
	plot_->addItem(rangeRectangle1_);
	rangeRectangle1_->setVisible(false);
	rangeRectangle2_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangle2_->setSelectable(false);
	rangeRectangle2_->setIgnoreWhenAutoScaling(true);
	rangeRectangle2_->setZValue(3000);
	plot_->addItem(rangeRectangle2_);
	rangeRectangle2_->setVisible(false);
	plot_->legend()->enableDefaultLegend(false);

	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());

	QVBoxLayout* vl = new QVBoxLayout();
	QFormLayout* fl = new QFormLayout();
	fl->addRow("Input:", names_);
	fl->addRow("Sum", axisSelector_);
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(rangeMinControl_);
	hl->addWidget(new QLabel("To"));
	hl->addWidget(rangeMaxControl_);
	fl->addRow("From (index)", hl);
	vl->addLayout(fl);
	vl->addWidget(plotWidget_);
	setLayout(vl);

	onAnalysisBlockInputDataSourcesChanged();

	// This needs to be called last because it requires all of the pointers to be valid.  All of dem.
	if (analysisBlock_->inputDataSourceCount() > 0)
		onNameChoiceChanged(0);

	// make connections
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));

//	connect(axisSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSumAxisControlChanged(int)));
	connect(rangeMinControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinControlChanged(int)));
	connect(rangeMaxControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxControlChanged(int)));
}

void AM3DBinningABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;

	for (int i = 0; i < analysisBlock_->inputDataSourceCount(); i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name())
			names_->addItem(tempSource->description(), tempSource->name());
	}
}

void AM3DBinningABEditor::onNameChoiceChanged(int index)
{
	QString name = names_->itemData(index).toString();
	analysisBlock_->setAnalyzedName(name);
	updateSeriesData();
}

void AM3DBinningABEditor::updateSeriesData()
{
	AMDataSource *source = analysisBlock_->inputDataSourceAt(0);

	if (source){

		int sumAxis = analysisBlock_->sumAxis();
		int sumAxisSize = source->size(sumAxis);
		QVector<double> xAxis(sumAxisSize);
		QVector<double> yAxis(sumAxisSize, 0);

		for (int i = 0; i < sumAxisSize; i++)
			xAxis[i] = double(source->axisValue(sumAxis, i));

		switch (sumAxis){

		case 0:{

			for (int i = 0, iSize = analysisBlock_->size(0); i < iSize; i++){

				for (int j = 0, jSize = analysisBlock_->size(1); j < jSize; j++){

					QVector<double> temp(sumAxisSize);
					source->values(AMnDIndex(0, i, j), AMnDIndex(sumAxisSize-1, i, j), temp.data());

					for (int k = 0; k < sumAxisSize; k++)
						yAxis[k] += temp.at(k);
				}
			}

			break;
		}

		case 1:{

			for (int i = 0, iSize = analysisBlock_->size(0); i < iSize; i++){

				for (int j = 0, jSize = analysisBlock_->size(1); j < jSize; j++){

					QVector<double> temp(sumAxisSize);
					source->values(AMnDIndex(i, 0, j), AMnDIndex(i, sumAxisSize-1, j), temp.data());

					for (int k = 0; k < sumAxisSize; k++)
						yAxis[k] += temp.at(k);
				}
			}

			break;
		}

		case 2:{

			for (int i = 0, iSize = analysisBlock_->size(0); i < iSize; i++){

				for (int j = 0, jSize = analysisBlock_->size(1); j < jSize; j++){

					QVector<double> temp(sumAxisSize);
					source->values(AMnDIndex(i, j, 0), AMnDIndex(i, j, sumAxisSize-1), temp.data());

					for (int k = 0; k < sumAxisSize; k++)
						yAxis[k] += temp.at(k);
				}
			}

			break;
		}
		}

		seriesData_->setValues(xAxis, yAxis);
	}
}

void AM3DBinningABEditor::onAnalysisBlockInputDataSourcesChanged()
{
	AMDataSource* inputSource;

	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		// inputSource is a valid data source

		axisSelector_->setEnabled(false);
		rangeMinControl_->setEnabled(true);
		rangeMaxControl_->setEnabled(true);

		QList<AMAxisInfo> inputSourceAxes = inputSource->axes();
		// we know (according to AM3DBinningAB's check of input source validity) that there are two axes here.

		axisSelector_->setItemText(0, inputSourceAxes.at(0).name + ": " + inputSourceAxes.at(0).description);
		axisSelector_->setItemText(1, inputSourceAxes.at(1).name + ": " + inputSourceAxes.at(1).description);
		axisSelector_->setItemText(2, inputSourceAxes.at(2).name + ": " + inputSourceAxes.at(2).description);

		// set the current status of the controls to reflect the (but don't have then trigger our slots)
		int sumAxis = analysisBlock_->sumAxis();
		axisSelector_->blockSignals(true);
		axisSelector_->setCurrentIndex(sumAxis);
		axisSelector_->blockSignals(false);

		rangeMinControl_->blockSignals(true);
		rangeMinControl_->setMaximum(inputSourceAxes.at(sumAxis).size-1);
		rangeMinControl_->setValue(analysisBlock_->sumRangeMin());
		rangeMinControl_->blockSignals(false);

		rangeMaxControl_->blockSignals(true);
		rangeMaxControl_->setMaximum(inputSourceAxes.at(sumAxis).size-1);
		rangeMaxControl_->setValue(analysisBlock_->sumRangeMax());
		rangeMaxControl_->blockSignals(false);

		updateSeriesData();
	}

	else {
		// no input source. Not much we can do.
		axisSelector_->setEnabled(false);
		rangeMinControl_->setEnabled(false);
		rangeMaxControl_->setEnabled(false);
	}

	placeRangeRectangle();
}

// responds to GUI events
void AM3DBinningABEditor::onSumAxisControlChanged(int newSumAxis)
{
	if(newSumAxis == analysisBlock_->sumAxis())
		return;

	// adjust the maximums for the ranges

	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		int newRangeMax = inputSource->size(newSumAxis)-1;
		rangeMinControl_->setMinimum(newRangeMax);
		rangeMaxControl_->setMaximum(newRangeMax);
		analysisBlock_->setSumRangeMin(0);
		analysisBlock_->setSumRangeMax(newRangeMax);
		rangeMinControl_->setValue(0);
		rangeMaxControl_->setValue(newRangeMax);
	}

	analysisBlock_->setSumAxis(newSumAxis);

	placeRangeRectangle();
	updateSeriesData();
}

void AM3DBinningABEditor::onRangeMinControlChanged(int newRangeMin)
{
	if(newRangeMin == analysisBlock_->sumRangeMin())
		return;

	analysisBlock_->setSumRangeMin(newRangeMin);

	// don't let the max go below the min.
	if(rangeMaxControl_->value() < newRangeMin)
		rangeMaxControl_->setValue(newRangeMin);

	placeRangeRectangle();
}

void AM3DBinningABEditor::onRangeMaxControlChanged(int newRangeMax)
{
	if(newRangeMax == analysisBlock_->sumRangeMax())
		return;

	analysisBlock_->setSumRangeMax(newRangeMax);

	if(rangeMinControl_->value() > newRangeMax)
		rangeMinControl_->setValue(newRangeMax);

	placeRangeRectangle();
}

void AM3DBinningABEditor::placeRangeRectangle()
{
	AMDataSource* inputSource;

	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		int sumAxis = analysisBlock_->sumAxis();

		double dataMin = inputSource->axisValue(sumAxis,0);
		double sumMin = inputSource->axisValue(sumAxis, analysisBlock_->sumRangeMin());
		double sumMax = inputSource->axisValue(sumAxis, analysisBlock_->sumRangeMax());
		double dataMax = inputSource->axisValue(sumAxis, inputSource->size(sumAxis)-1);

		rangeRectangle1_->setYAxisTarget(plot_->axisScaleVerticalRelative());	// note: does nothing if already correct
		rangeRectangle2_->setYAxisTarget(plot_->axisScaleVerticalRelative());	// note: does nothing if already correct
		rangeRectangle1_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct
		rangeRectangle2_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct

		rangeRectangle1_->setRect(QRectF(QPointF(dataMin,0), QPointF(sumMin,1)).normalized());
		rangeRectangle2_->setRect(QRectF(QPointF(sumMax,0), QPointF(dataMax,1)).normalized());

		rangeRectangle1_->setVisible(true);
		rangeRectangle2_->setVisible(true);
	}

	else {

		rangeRectangle1_->setVisible(false);
		rangeRectangle2_->setVisible(false);
	}
}
