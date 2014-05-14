#include "STModel.h"

STModel::STModel(QObject *parent) :
    QObject(parent)
{
    variables_ = new STVariableCollection(this);
    selectedVariables_ = new STVariableCollection(this);
}

STModel::~STModel()
{

}

STVariableCollection* STModel::variables() const
{
    return variables_;
}

STVariableCollection* STModel::selectedVariables() const
{
    return selectedVariables_;
}

void STModel::addVariable(STVariable *toAdd)
{
    variables_->addVariable(toAdd);
}

void STModel::removeVariable(STVariable *toRemove)
{
    variables_->removeVariable(toRemove);
}

void STModel::addVariableToSelection(STVariable *toAdd)
{
    selectedVariables_->addVariable(toAdd);
}

void STModel::removeVariableFromSelection(STVariable *toRemove)
{
    selectedVariables_->removeVariable(toRemove);
}

void STModel::clearSelection()
{
    selectedVariables_->clearCollection();
}
