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


#include "AM2DSummingABEditor.h"

#include "analysis/AM2DSummingAB.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>

AM2DSummingABEditor::AM2DSummingABEditor(AM2DSummingAB* analysisBlock, QWidget *parent) :
	QWidget(parent)
{

	analysisBlock_ = analysisBlock;

	axisSelector_ = new QComboBox();
	axisSelector_->addItem("Horizontal");
	axisSelector_->addItem("Vertical");

	rangeMinControl_ = new QSpinBox();
	rangeMinControl_->setSingleStep(1);
	rangeMinControl_->setMinimum(0);
	rangeMaxControl_ = new QSpinBox();
	rangeMaxControl_->setSingleStep(1);
	rangeMaxControl_->setMinimum(0);

	additionalEditor_ = 0;


	QFormLayout* fl = new QFormLayout();
	fl->addRow("Sum", axisSelector_);
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(rangeMinControl_);
	hl->addWidget(new QLabel("To"));
	hl->addWidget(rangeMaxControl_);
	fl->addRow("From (index)", hl);
	setLayout(fl);

	onAnalysisBlockInputDataSourcesChanged();

	// make connections
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(onAnalysisBlockInputDataSourcesChanged()));

	connect(axisSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSumAxisControlChanged(int)));
	connect(rangeMinControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMinControlChanged(int)));
	connect(rangeMaxControl_, SIGNAL(valueChanged(int)), this, SLOT(onRangeMaxControlChanged(int)));
}


void AM2DSummingABEditor::onAnalysisBlockInputDataSourcesChanged() {

	AMDataSource* inputSource;
	if(analysisBlock_->inputDataSourceCount() > 0 && (inputSource=analysisBlock_->inputDataSourceAt(0))) {

		// inputSource is a valid data source

		axisSelector_->setEnabled(true);
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
	}

	else {
		// no input source. Not much we can do.
		axisSelector_->setEnabled(false);
		rangeMinControl_->setEnabled(false);
		rangeMaxControl_->setEnabled(false);
	}
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
}

void AM2DSummingABEditor::onRangeMinControlChanged(int newRangeMin) {
	if(newRangeMin == analysisBlock_->sumRangeMin())
		return;

	analysisBlock_->setSumRangeMin(newRangeMin);

	// don't let the max go below the min.
	if(rangeMaxControl_->value() < newRangeMin)
		rangeMaxControl_->setValue(newRangeMin);
}

void AM2DSummingABEditor::onRangeMaxControlChanged(int newRangeMax) {

	if(newRangeMax == analysisBlock_->sumRangeMax())
		return;

	analysisBlock_->setSumRangeMax(newRangeMax);

	if(rangeMinControl_->value() > newRangeMax)
		rangeMinControl_->setValue(newRangeMax);
}
