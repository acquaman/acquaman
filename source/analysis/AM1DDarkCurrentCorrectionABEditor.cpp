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

