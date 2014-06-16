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


#include "AM1DBasicIntegralABEditor.h"

#include <QLabel>
#include <QHBoxLayout>

 AM1DBasicIntegralABEditor::~AM1DBasicIntegralABEditor(){}
AM1DBasicIntegralABEditor::AM1DBasicIntegralABEditor(AM1DIntegralAB *analysisBlock, QWidget *parent)
	: QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	names_ = new QComboBox;
	populateComboBox();

	connect(names_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNameChoiceChanged(int)));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));

	if (analysisBlock_->inputDataSourceCount() > 0)
		onNameChoiceChanged(0);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel("Input:"), 0, Qt::AlignRight);
	layout->addWidget(names_, 0, Qt::AlignLeft);

	setLayout(layout);
}

void AM1DBasicIntegralABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;

	for (int i = 0; i < analysisBlock_->inputDataSourceCount(); i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name() && !tempSource->hiddenFromUsers())
			names_->addItem(tempSource->description(), tempSource->name());
	}
}

void AM1DBasicIntegralABEditor::onNameChoiceChanged(int index)
{
	QString name = names_->itemData(index).toString();
	analysisBlock_->setAnalyzedName(name);
}
