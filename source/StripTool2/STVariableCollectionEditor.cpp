#include "STVariableCollectionEditor.h"

STVariableCollectionEditor::STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent) :
    STEditor(parent)
{
    // Set variables.

    variableCollection_ = collection;

    // Create UI.

    variableList_ = new STListView(this);

    variableEditors_ = new STVariableEditors(0, this);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(variableList_);
    mainLayout->addWidget(variableEditors_);
    setLayout(mainLayout);

    // Get current settings.

    variableList_->setModel(variableCollection_);
    variableList_->setMaximumHeight(variableList_->sizeHintForRow(0) * 5);
    onCollectionSelection(variableCollection_->selectedVariable());

    // Make connections

    connect( variableCollection_, SIGNAL(selectionChanged(STVariable*)), this, SLOT(onCollectionSelection(STVariable*)) );
    connect( variableList_, SIGNAL(clicked(QModelIndex)), this, SLOT(onListViewSelection(QModelIndex)) );
    connect( variableList_, SIGNAL(addVariable()), this, SIGNAL(addVariable()) );
    connect( variableEditors_, SIGNAL(removeButtonClicked()), this, SLOT(removeSelectedVariable()) );
}

STVariableCollectionEditor::~STVariableCollectionEditor()
{

}

STVariableCollection* STVariableCollectionEditor::variableCollection() const
{
    return variableCollection_;
}

void STVariableCollectionEditor::applyChanges()
{
    variableEditors_->applyChanges();
}

void STVariableCollectionEditor::onListViewSelection(QModelIndex selection)
{
    STVariable *listSelection = variableCollection_->variableAt(selection.row());
    variableCollection_->setSelectedVariable(listSelection);
    variableEditors_->setVariable(listSelection);
}

void STVariableCollectionEditor::removeSelectedVariable()
{
    if (variableCollection_->selectedVariable()) {
        int indexToDelete = variableCollection()->selectedVariable()->index();
        variableCollection_->deleteVariableAt(indexToDelete);

        if (variableCollection_->variableCount() == 0)
            emit closeDialog();
    }
}

void STVariableCollectionEditor::onCollectionSelection(STVariable *selection)
{
    variableList_->setSelectedIndices(QModelIndexList() << variableCollection_->indexOf(selection));
    variableEditors_->setVariable(selection);
}
