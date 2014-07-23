#include "STVariableEditorAdvanced.h"

STVariableEditorAdvanced::STVariableEditorAdvanced(STVariable *toEdit, QWidget *parent) :
    STVariableEditor(toEdit, parent)
{
    // Set variables.

    pointsSavedEdited_ = false;

    // Create UI.

    QGridLayout *mainLayout = new QGridLayout();

    QLabel *pointsSavedLabel = new QLabel("Points saved: ", this);
    pointsSavedEntry_ = new QSpinBox(this);
    pointsSavedEntry_->setMinimum(1);
    pointsSavedEntry_->setMaximum(1000000);
    pointsSavedLabel->setBuddy(pointsSavedEntry_);
    mainLayout->addWidget(pointsSavedLabel, 0, 0);
    mainLayout->addWidget(pointsSavedEntry_, 0, 1);

    QVBoxLayout *stretch = new QVBoxLayout();
    stretch->addStretch();
    mainLayout->addLayout(stretch, 1, 0);

    setLayout(mainLayout);

    // Get current settings.

    pointsSavedEntry_->setEnabled(false);

    getVariableInfo();

    // Make connections.

    connectUI();
}

STVariableEditorAdvanced::~STVariableEditorAdvanced()
{

}

void STVariableEditorAdvanced::applyChanges()
{
    if (variable_) {
        if (pointsSavedEdited_) {
            variable_->setPointsSaved(pointsSavedEntry_->value());
            pointsSavedEdited_ = false;
        }
    }
}

void STVariableEditorAdvanced::onPointsSavedEntryChanged()
{
    pointsSavedEdited_ = true;
    applyChanges();
}

void STVariableEditorAdvanced::getVariableInfo()
{
    if (variable_) {
        pointsSavedEntry_->setEnabled(true);
        pointsSavedEntry_->setValue(variable_->pointsSaved());
    }
}

void STVariableEditorAdvanced::clearVariableInfo()
{
    pointsSavedEntry_->clear();
    pointsSavedEntry_->setEnabled(false);

    pointsSavedEdited_ = false;
}

void STVariableEditorAdvanced::connectUI()
{
    connect( pointsSavedEntry_, SIGNAL(editingFinished()), this, SLOT(onPointsSavedEntryChanged()) );
}

void STVariableEditorAdvanced::disconnectUI()
{
    disconnect( pointsSavedEntry_, SIGNAL(editingFinished()), this, SLOT(onPointsSavedEntryChanged()) );
}
