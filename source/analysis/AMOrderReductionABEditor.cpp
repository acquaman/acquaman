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


#include "AMOrderReductionABEditor.h"

#include <QFormLayout>

 AMOrderReductionABEditor::~AMOrderReductionABEditor(){}
AMOrderReductionABEditor::AMOrderReductionABEditor(AMOrderReductionAB *analysisBlock, QWidget *parent)
	: QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	sourceNames_ = new QComboBox;
	reducedAxisOptions_ = new QComboBox;
	populateComboBox();

	connect(sourceNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataSourceChanged(int)));
	connect(reducedAxisOptions_, SIGNAL(currentIndexChanged(int)), this, SLOT(onReducedAxisChanged(int)));

	if (analysisBlock_->inputDataSourceCount() > 0){

		sourceNames_->setCurrentIndex(sourceNames_->findData(analysisBlock_->selectedName()));
		reducedAxisOptions_->setCurrentIndex(reducedAxisOptions_->findData(analysisBlock_->reducedAxis()));
	}

	QFormLayout *layout = new QFormLayout;
	layout->addRow("Source:", sourceNames_);
	layout->addRow("Reduced Axis:", reducedAxisOptions_);

	setLayout(layout);
}

void AMOrderReductionABEditor::onDataSourceChanged(int index)
{
	analysisBlock_->setSelectedName(sourceNames_->itemData(index).toString());
}

void AMOrderReductionABEditor::onReducedAxisChanged(int index)
{
	QVariant val = reducedAxisOptions_->itemData(index);

	if (val.isValid())
		analysisBlock_->setReducedAxis(val.toInt());

	else
		analysisBlock_->setReducedAxis(-1);
}

void AMOrderReductionABEditor::populateComboBox()
{
	AMDataSource *source = 0;

	for (int i = 0, size = analysisBlock_->inputDataSourceCount(); i < size; i++){

		source = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != source->name())
			sourceNames_->addItem(source->description(), source->name());
	}

	if (analysisBlock_->rank() != 0){

		for (int i = 0, size = analysisBlock_->currentInputSource()->rank(); i < size; i++)
			reducedAxisOptions_->insertItem(i, analysisBlock_->currentInputSource()->axisInfoAt(i).description, i);
	}
}
