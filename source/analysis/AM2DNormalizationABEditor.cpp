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


#include "AM2DNormalizationABEditor.h"

#include <QLabel>
#include <QFormLayout>

 AM2DNormalizationABEditor::~AM2DNormalizationABEditor(){}
AM2DNormalizationABEditor::AM2DNormalizationABEditor(AM2DNormalizationAB *analysisBlock, QWidget *parent)
	: QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	dataNames_ = new QComboBox;
	normalizationNames_ = new QComboBox;
	populateComboBox();

	connect(dataNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataNameChoiceChanged(int)));
	connect(normalizationNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNormalizationNameChoiceChanged(int)));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));

	if (analysisBlock_->inputDataSourceCount() > 0){

		dataNames_->setCurrentIndex(dataNames_->findData(analysisBlock_->dataName()));
		normalizationNames_->setCurrentIndex(normalizationNames_->findData(analysisBlock_->normalizationName()));
	}

	QFormLayout *layout = new QFormLayout;
	layout->addRow("Data:", dataNames_);
	layout->addRow("Normalization:", normalizationNames_);

	setLayout(layout);
}

void AM2DNormalizationABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;
	int inputCount = analysisBlock_->inputDataSourceCount();

	for (int i = 0; i < inputCount; i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name()){

			dataNames_->addItem(tempSource->description(), tempSource->name());
			normalizationNames_->addItem(tempSource->description(), tempSource->name());
		}
	}
}

void AM2DNormalizationABEditor::onDataNameChoiceChanged(int index)
{
	QString name = dataNames_->itemData(index).toString();
	analysisBlock_->setDataName(name);
}

void AM2DNormalizationABEditor::onNormalizationNameChoiceChanged(int index)
{
	QString name = normalizationNames_->itemData(index).toString();
	analysisBlock_->setNormalizationName(name);
}
