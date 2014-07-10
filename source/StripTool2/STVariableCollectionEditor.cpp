#include "STVariableCollectionEditor.h"

STVariableCollectionEditor::STVariableCollectionEditor(STVariableCollection *collection, QWidget *parent) :
    STEditor(parent)
{
    // Set variables.

    variableCollection_ = collection;

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

    if (variableCollection_->selectedVariable())
        variableList_->setCurrentIndex(variableCollection_->indexOf(variableCollection_->selectedVariable()));

    // Make connections

    connect( variableList_, SIGNAL(clicked(QModelIndex)), this, SLOT(onListViewSelection(QModelIndex)) );
    connect( variableList_, SIGNAL(addVariable()), this, SIGNAL(addVariable()) );
    connect( variableEditor_, SIGNAL(removeButtonClicked()), this, SLOT(removeSelectedVariable()) );
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
    variableEditor_->applyChanges();
}

void STVariableCollectionEditor::onListViewSelection(QModelIndex selection)
{
    STVariable *listSelection = variableCollection_->variableAt(selection.row());
    variableCollection_->setSelectedVariable(listSelection);
    variableEditor_->setVariable(listSelection);
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
