#include "AM1DBasicIntegralABEditor.h"

#include <QLabel>
#include <QHBoxLayout>

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
