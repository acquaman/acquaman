#include "STVariableCollectionEditor.h"

STVariableCollectionEditor::STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent) :
    STEditor(parent)
{
    // Set variables.

    variableCollection_ = collection;

    selectedVariable_ = 0;

    // Set up UI.

    QVBoxLayout *mainLayout = new QVBoxLayout();

    variableList_ = new QListView(this);
    connect( variableList_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onListViewSelection(QModelIndex, QModelIndex)) );

    variableList_->setModel(variableCollection_);
    mainLayout->addWidget(variableList_);

    variableEditor_ = new STVariableEditor(0);
    mainLayout->addWidget(variableEditor_);

    setLayout(mainLayout);
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

void STVariableCollectionEditor::onListViewSelection(QModelIndex start, QModelIndex end)
{
    Q_UNUSED(end)

    setSelectedVariable(variableCollection_->variableAt(start.row()) );
}

void STVariableCollectionEditor::setSelectedVariable(STVariable *newSelection)
{
    if (selectedVariable_ != newSelection) {
        selectedVariable_ = newSelection;

        variableEditor_->setVariable(selectedVariable_);

        emit selectedVariableChanged(selectedVariable_);
    }
}
