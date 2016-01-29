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


#include "AM2DSummingABEditor.h"

#include "analysis/AM2DSummingAB.h"

#include <QFormLayout>
#include <QBoxLayout>
#include <QLabel>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceImageData.h"

 AM2DSummingABEditor::~AM2DSummingABEditor(){}
AM2DSummingABEditor::AM2DSummingABEditor(AM2DSummingAB* analysisBlock, QWidget *parent) :
	QWidget(parent)
{

	analysisBlock_ = analysisBlock;

	names_ = new QComboBox;
	populateComboBox();

	connect(names_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNameChoiceChanged(int)));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));

	axisSelector_ = new QComboBox();
	axisSelector_->addItem("Horizontal");
	axisSelector_->addItem("Vertical");

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
	image_ = 0;

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
	if (analysisBlock_->inputDataSourceCount() > 0){
		if(analysisBlock_->analyzedName().isEmpty())
			onNameChoiceChanged(0);
		else{
			for(int x = 0; x < names_->count(); x++)
				if(names_->itemData(x).toString() == analysisBlock_->analyzedName())
					names_->setCurrentIndex(x);
		}
	}

	// make connections
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));

//	connect(axisSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSumAxisControlChanged(int)));
	connect(rangeMinControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinControlChanged(int)));
	connect(rangeMaxControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxControlChanged(int)));
}

void AM2DSummingABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;

	for (int i = 0; i < analysisBlock_->inputDataSourceCount(); i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name() && !tempSource->hiddenFromUsers())
			names_->addItem(tempSource->description(), tempSource->name());
	}
}

void AM2DSummingABEditor::onNameChoiceChanged(int index)
{
	QString name = names_->itemData(index).toString();
	analysisBlock_->setAnalyzedName(name);
	AMDataSourceImageData *model = new AMDataSourceImageData;
	model->setDataSource(analysisBlock_->inputDataSourceAt(analysisBlock_->indexOfInputSource(name)));
	image_->setModel(model, true);
}


void AM2DSummingABEditor::onAnalysisBlockInputDataSourcesChanged() {

	if(image_) {
		delete image_;
		image_ = 0;
	}

	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		// inputSource is a valid data source

		axisSelector_->setEnabled(false);
		rangeMinControl_->setEnabled(true);
		rangeMaxControl_->setEnabled(true);

		QList<AMAxisInfo> inputSourceAxes = inputSource->axes();
		// we know (according to AM2DSummingAB's check of input source validity) that there are two axes here.

		axisSelector_->setItemText(0, inputSourceAxes.at(0).name + ": " + inputSourceAxes.at(0).description);
		axisSelector_->setItemText(1, inputSourceAxes.at(1).name + ": " + inputSourceAxes.at(1).description);

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

		image_ = new MPlotImageBasic();
		AMDataSourceImageData *model = new AMDataSourceImageData;
		model->setDataSource(inputSource);
		image_->setModel(model, true);
		plot_->addItem(image_);
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
void AM2DSummingABEditor::onSumAxisControlChanged(int newSumAxis) {

	if(newSumAxis == analysisBlock_->sumAxis())
		return;

	// adjust the maximums for the ranges

	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		int newRangeMax = inputSource->size(newSumAxis)-1;
		rangeMinControl_->setMaximum(newRangeMax);
		rangeMaxControl_->setMaximum(newRangeMax);
	}

	analysisBlock_->setSumAxis(newSumAxis);

	placeRangeRectangle();
}

void AM2DSummingABEditor::onRangeMinControlChanged(int newRangeMin) {
	if(newRangeMin == analysisBlock_->sumRangeMin())
		return;

	analysisBlock_->setSumRangeMin(newRangeMin);

	// don't let the max go below the min.
	if(rangeMaxControl_->value() < newRangeMin)
		rangeMaxControl_->setValue(newRangeMin);

	placeRangeRectangle();
}

void AM2DSummingABEditor::onRangeMaxControlChanged(int newRangeMax) {

	if(newRangeMax == analysisBlock_->sumRangeMax())
		return;

	analysisBlock_->setSumRangeMax(newRangeMax);

	if(rangeMinControl_->value() > newRangeMax)
		rangeMinControl_->setValue(newRangeMax);

	placeRangeRectangle();
}

void AM2DSummingABEditor::placeRangeRectangle()
{
	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		int sumAxis = analysisBlock_->sumAxis();

		double dataMin = inputSource->axisValue(sumAxis,0);
		double sumMin = inputSource->axisValue(sumAxis, analysisBlock_->sumRangeMin());
		double sumMax = inputSource->axisValue(sumAxis, analysisBlock_->sumRangeMax());
		double dataMax = inputSource->axisValue(sumAxis, inputSource->size(sumAxis)-1);

		if(sumAxis == 0) {

			rangeRectangle1_->setYAxisTarget(plot_->axisScaleVerticalRelative());	// note: does nothing if already correct
			rangeRectangle2_->setYAxisTarget(plot_->axisScaleVerticalRelative());	// note: does nothing if already correct
			rangeRectangle1_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct
			rangeRectangle2_->setXAxisTarget(plot_->axisScaleBottom());	// note: does nothing if already correct

			rangeRectangle1_->setRect(QRectF(QPointF(dataMin,0), QPointF(sumMin,1)).normalized());
			rangeRectangle2_->setRect(QRectF(QPointF(sumMax,0), QPointF(dataMax,1)).normalized());
		}

		else {
			rangeRectangle1_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
			rangeRectangle2_->setYAxisTarget(plot_->axisScaleLeft());	// note: does nothing if already correct
			rangeRectangle1_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct
			rangeRectangle2_->setXAxisTarget(plot_->axisScaleHorizontalRelative());	// note: does nothing if already correct

			rangeRectangle1_->setRect(QRectF(QPointF(0,dataMin), QPointF(1,sumMin)).normalized());
			rangeRectangle2_->setRect(QRectF(QPointF(0,sumMax), QPointF(1,dataMax)).normalized());
		}
		rangeRectangle1_->setVisible(true);
		rangeRectangle2_->setVisible(true);
	}
	else {
		rangeRectangle1_->setVisible(false);
		rangeRectangle2_->setVisible(false);
	}
}
