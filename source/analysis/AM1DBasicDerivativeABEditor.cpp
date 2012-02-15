#include "AM1DBasicDerivativeABEditor.h"

#include <QLabel>
#include <QHBoxLayout>

AM1DBasicDerivativeABEditor::AM1DBasicDerivativeABEditor(AM1DDerivativeAB *derivative, QWidget *parent)
	: QWidget(parent)
{
	derivative_ = derivative;

	names_ = new QComboBox;
	populateComboBox();

	connect(names_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNameChoiceChanged(int)));
	connect(derivative_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));

	if (derivative_->inputDataSourceCount() > 0)
		onNameChoiceChanged(0);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel("Input:"), 0, Qt::AlignRight);
	layout->addWidget(names_, 0, Qt::AlignLeft);

	setLayout(layout);
}

void AM1DBasicDerivativeABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;

	for (int i = 0; i < derivative_->inputDataSourceCount(); i++){

		tempSource = derivative_->inputDataSourceAt(i);

		if (derivative_->name() != tempSource->name() && !tempSource->hiddenFromUsers())
			names_->addItem(tempSource->description(), tempSource->name());
	}
}

void AM1DBasicDerivativeABEditor::onNameChoiceChanged(int index)
{
	QString name = names_->itemData(index).toString();
	derivative_->setAnalyzedName(name);
}
