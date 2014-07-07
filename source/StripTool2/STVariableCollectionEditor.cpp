#include "STVariableCollectionEditor.h"

STVariableCollectionEditor::STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent) :
    STEditor(parent)
{
    // Set variables.

    variableCollection_ = collection;
    selectedVariable_ = 0;

    // Create UI.

    QVBoxLayout *mainLayout = new QVBoxLayout();

    variableList_ = new STListView(this);
    mainLayout->addWidget(variableList_);

    variableEditor_ = new STVariableEditor(0);
    mainLayout->addWidget(variableEditor_);

    setLayout(mainLayout);

    // Get current settings.

    variableList_->setModel(variableCollection_);
    variableList_->setMaximumHeight(variableList_->sizeHintForRow(0) * 5);

    variableEditor_->setEnabled(false);

    // Make connections

    connect( variableList_, SIGNAL(clicked(QModelIndex)), this, SLOT(onListViewSelection(QModelIndex)) );
    connect( variableList_, SIGNAL(addVariable()), this, SIGNAL(addVariable()) );
    connect( variableEditor_, SIGNAL(removeButtonClicked()), this, SLOT(onRemoveButtonClicked()) );
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

void STVariableCollectionEditor::onRemoveButtonClicked()
{
    if (selectedVariable_) {
        qDebug() << "Removing selected variable...";
        int indexToDelete = selectedVariable_->index();
        setSelectedVariable(0);
        variableCollection_->deleteVariableAt(indexToDelete);

    } else {
        // There shouldn't be a remove button visible if there is no selected variable!!
        qDebug() << "ERROR : attempting to remove a non-selected variable!";
    }
}
