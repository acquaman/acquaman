#include "STVariableEditor.h"

STVariableEditor::STVariableEditor(STVariable *toEdit, QWidget *parent) : QWidget(parent)
{
    variable_ = toEdit;
}

STVariableEditor::~STVariableEditor()
{

}

STVariable* STVariableEditor::variable() const
{
    return variable_;
}

void STVariableEditor::setVariable(STVariable *toEdit)
{
    if (variable_) {
        disconnectUI();
        disconnectVariable();
        clearVariableInfo();
        variable_ = 0;
    }

    variable_ = toEdit;
    getVariableInfo();
    connectUI();
    connectVariable();

    emit variableChanged(variable_);
}
