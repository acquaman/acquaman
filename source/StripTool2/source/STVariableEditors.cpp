#include "STVariableEditors.h"

STVariableEditors::STVariableEditors(STVariable *toEdit, QWidget *parent) :
    QTabWidget(parent)
{    
    basicEditor_ = new STVariableEditorBasic(toEdit);
    connect( basicEditor_, SIGNAL(removeButtonClicked()), this, SIGNAL(removeButtonClicked()) );
    addTab(qobject_cast<QWidget*>(basicEditor_), "Basic");

    plotEditor_ = new STVariableEditorPlot(toEdit);
    addTab(qobject_cast<QWidget*>(plotEditor_), "Plot");

    advancedEditor_ = new STVariableEditorAdvanced(toEdit);
    addTab(qobject_cast<QWidget*>(advancedEditor_), "Advanced");
}

STVariableEditors::~STVariableEditors()
{

}

QList<STVariableEditor*> STVariableEditors::editors() const
{
    return QList<STVariableEditor*>() << basicEditor_ << plotEditor_ << advancedEditor_;
}

STVariableEditor* STVariableEditors::basicEditor() const
{
    return basicEditor_;
}

STVariableEditor* STVariableEditors::plotEditor() const
{
    return plotEditor_;
}

STVariableEditor* STVariableEditors::advancedEditor() const
{
    return advancedEditor_;
}

void STVariableEditors::setVariable(STVariable *newVariable)
{
    basicEditor_->setVariable(newVariable);
    plotEditor_->setVariable(newVariable);
    advancedEditor_->setVariable(newVariable);
}

void STVariableEditors::applyChanges()
{
    foreach (STVariableEditor *editor, editors()) {
        editor->applyChanges();
    }
}
