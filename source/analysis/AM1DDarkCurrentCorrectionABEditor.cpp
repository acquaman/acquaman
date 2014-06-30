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


#include "AM1DDarkCurrentCorrectionABEditor.h"

AM1DDarkCurrentCorrectionABEditor::AM1DDarkCurrentCorrectionABEditor(AM1DDarkCurrentCorrectionAB *analysisBlock, QWidget *parent)
    : QWidget(parent)
{
    analysisBlock_ = analysisBlock;

    dataNames_ = new QComboBox();
    populateComboBox();

    connect( dataNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataNameChoiceChanged(int)) );
    connect( analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()) );

    if (analysisBlock_->inputDataSourceCount() > 0) {
        onDataNameChoiceChanged(0);
    }

    QFormLayout *layout = new QFormLayout();
    layout->addRow("Input:", dataNames_);

    setLayout(layout);
}

AM1DDarkCurrentCorrectionABEditor::~AM1DDarkCurrentCorrectionABEditor() {}

void AM1DDarkCurrentCorrectionABEditor::populateComboBox()
{
    AMDataSource *tempSource = 0;
    int inputCount = analysisBlock_->inputDataSourceCount();

    for (int i = 0; i < inputCount; i++) {
        tempSource = analysisBlock_->inputDataSourceAt(i);

        if (analysisBlock_->name() != tempSource->name() && !tempSource->hiddenFromUsers()) {
            dataNames_->addItem(tempSource->description(), tempSource->name());
        }
    }
}

void AM1DDarkCurrentCorrectionABEditor::onDataNameChoiceChanged(int index)
{
    QString name = dataNames_->itemData(index).toString();
    analysisBlock_->setDataName(name);
}

