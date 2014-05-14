#include "STVariableCollection.h"

STVariableCollection::STVariableCollection(QObject *parent) :
    QObject(parent)
{
    variables_ = new QList<STVariable*>();
}

STVariableCollection::~STVariableCollection()
{

}

QList<STVariable*>* STVariableCollection::variables() const
{
    return variables_;
}

void STVariableCollection::addVariable(STVariable *toAdd)
{
    variables_->append(toAdd);
    emit variableAdded();
}

void STVariableCollection::removeVariable(STVariable *toRemove)
{
    if (variables_->contains(toRemove)) {
        variables_->removeOne(toRemove);
        emit variableRemoved();
    }

    if (!variables_->contains(toRemove))
        emit variableAllRemoved();
}

void STVariableCollection::removeAllVariable(STVariable *toRemove)
{
    if (variables_->contains(toRemove)) {
        variables_->removeAll(toRemove);
        emit variableAllRemoved();
    }
}

void STVariableCollection::clearCollection()
{
    variables_->clear();
    emit cleared();
}
