#include "STVariableCollection.h"

STVariableCollection::STVariableCollection(QObject *parent) :
    QObject(parent)
{
    connectedMapper_ = new QSignalMapper(this);
    connect( connectedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onVariableConnectedStateChanged(QObject*)) );
}

STVariableCollection::~STVariableCollection()
{

}

QList<STVariable*> STVariableCollection::variables() const
{
    return variables_;
}

int STVariableCollection::variableCount() const
{
    return variables_.size();
}

STVariable* STVariableCollection::variableAt(int index) const
{
    return variables_.at(index);
}

int STVariableCollection::indexOf(STVariable *toMatch) const
{
    return variables_.indexOf(toMatch);
}

QList<STVariable*> STVariableCollection::variablesWithName(const QString &name)
{
    QList<STVariable*> results;

    foreach (STVariable *variable, variables_) {
        if (variable->name() == name)
            results.append(variable);
    }

    return results;
}

void STVariableCollection::addVariable(const QString &name)
{
    if (name.isEmpty()) {
        emit variableAdded(false);
        return;
    }

    if (!variablesWithName(name).isEmpty()) {
        emit variableAdded(false);
        return;
    }

    STVariable *newVariable = new STVariable(name, this);
    variables_.append(newVariable);
    emit variableAdded(true);

    connectVariable(variables_.size() - 1);
}

void STVariableCollection::deleteVariableAt(int index)
{
    if (index < 0 || index >= variables_.size()) {
        emit variableDeleted(false);
        return;
    }

    STVariable *toDelete = variableAt(index);

    // if we can successfully remove the variable, disconnect it and queue it up for deletion.
    variables_.removeAt(index);
    disconnectVariable(toDelete);
    emit variableDeleted(true);
    toDelete->deleteLater();
}

void STVariableCollection::onVariableConnectedStateChanged(QObject *object)
{
    STVariable *variable = qobject_cast<STVariable*>(object);

    if (variable) {
        if (variable->isConnected()) {
            emit variableConnected(true, indexOf(variable));

        } else {
            emit variableConnected(false, indexOf(variable));
        }
    }
}

void STVariableCollection::connectVariable(int index)
{
    STVariable *toConnect = variableAt(index);

    connectedMapper_->setMapping(toConnect, toConnect);
    connect( toConnect, SIGNAL(connected(bool)), connectedMapper_, SLOT(map()) );
    if (toConnect->isConnected()) {
        emit variableConnected(true, index);
    }
}

void STVariableCollection::disconnectVariable(STVariable *variable)
{
    // do not have to worry about removing mappings--done automatically when variable is destroyed.

    Q_UNUSED(variable)
}
