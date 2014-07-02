#include "STVariableCollection.h"

STVariableCollection::STVariableCollection(QObject *parent) :
    QObject(parent)
{
    connectedMapper_ = new QSignalMapper(this);
    connect( connectedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onVariableConnectedStateChanged(STVariable*)) );
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
        emit variableAdded(false, name);
        return;
    }

    if (!variablesWithName(name).isEmpty()) {
        emit variableAdded(false, name);
        return;
    }

    STVariable *newVariable = new STVariable(name, this);
    variables_.append(newVariable);
    connectVariable(newVariable);
}

void STVariableCollection::deleteVariable(const QString &name)
{
    if (name.isEmpty()) {
        emit variableDeleted(false, name);
        return;
    }

    QList<STVariable*> toDelete = variablesWithName(name);

    foreach (STVariable *variable, toDelete) {
        disconnectVariable(variable);
        variables_.removeOne(variable);
        variable->deleteLater();
    }
}

void STVariableCollection::onVariableConnectedStateChanged(STVariable *variable)
{
    if (variable->isConnected()) {
        emit variableConnected(true, variable->name());

    } else {
        emit variableConnected(false, variable->name());
    }
}

void STVariableCollection::connectVariable(STVariable *variable)
{
    connectedMapper_->setMapping(variable, variable->name());
    connect( variable, SIGNAL(connected(bool)), connectedMapper_, SLOT(map()) );
    if (variable->isConnected()) {
        emit variableConnected(true, variable->name());
    }
}

void STVariableCollection::disconnectVariable(STVariable *variable)
{
    // do not have to worry about removing mappings--done automatically when variable is destroyed.

    Q_UNUSED(variable)
}
