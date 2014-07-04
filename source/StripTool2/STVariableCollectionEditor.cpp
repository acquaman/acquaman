#include "STVariableCollectionEditor.h"

STVariableCollectionEditor::STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent) :
    STEditor(parent)
{
    // Set variables.

    variableCollection_ = collection;

    selectedVariable_ = 0;

    // Create UI.

    QVBoxLayout *mainLayout = new QVBoxLayout();

    variableList_ = new QListView(this);

    variableList_->setModel(variableCollection_);
    mainLayout->addWidget(variableList_);

    variableEditor_ = new STVariableEditor(0);
    variableEditor_->setEnabled(false);
    mainLayout->addWidget(variableEditor_);

    setLayout(mainLayout);

    // Make connections

    connect( variableList_, SIGNAL(clicked(QModelIndex)), this, SLOT(onListViewSelection(QModelIndex)) );
}

STVariableCollectionEditor::~STVariableCollectionEditor()
{

}

STVariableCollection* STVariableCollectionEditor::variableCollection() const
{
    return variableCollection_;
}

STVariable* STVariableCollectionEditor::selectedVariable() const
{
    return selectedVariable_;
}

void STVariableCollectionEditor::applyChanges()
{
    variableEditor_->applyChanges();
}

void STVariableCollectionEditor::onListViewSelection(QModelIndex selection)
{
    setSelectedVariable(variableCollection_->variableAt(selection.row()) );
}

void STVariableCollectionEditor::setSelectedVariable(STVariable *newSelection)
{
    if (selectedVariable_ != newSelection) {
        selectedVariable_ = newSelection;

        variableEditor_->setEnabled(true);
        variableEditor_->setVariable(selectedVariable_);

        emit selectedVariableChanged(selectedVariable_);
    }
}
