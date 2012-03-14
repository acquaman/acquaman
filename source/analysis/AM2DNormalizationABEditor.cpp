#include "AM2DNormalizationABEditor.h"

#include <QLabel>
#include <QFormLayout>

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

	if (inputCount < 2)
		return;

	for (int i = 0; i < inputCount; i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name() && !tempSource->hiddenFromUsers()){

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
