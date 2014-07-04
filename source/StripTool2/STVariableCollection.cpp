#include "STVariableCollection.h"

STVariableCollection::STVariableCollection(QObject *parent) : QAbstractTableModel(parent)
{

}

STVariableCollection::~STVariableCollection()
{

}

int STVariableCollection::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return variables_.size();
}

int STVariableCollection::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant STVariableCollection::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return variables_.at(index.row())->name();
    }

    return QVariant();
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
        return;
    }

    STVariable *newVariable = new STVariable(name, this);
    variables_.append(newVariable);

    int index = variables_.size() - 1;
    newVariable->setIndex(index);

    connectVariable(newVariable);

    emit variableAdded(index);
}

void STVariableCollection::deleteVariableAt(int index)
{
    if (index < 0 || index >= variables_.size()) {
        return;
    }

    STVariable *toDelete = variableAt(index);

    // if we can successfully remove the variable, disconnect it and queue it up for deletion.
    variables_.removeAt(index);
    disconnectVariable(toDelete);

    emit variableDeleted();

    toDelete->deleteLater();
}

void STVariableCollection::onVariableConnectedStateChanged(bool isConnected)
{
    Q_UNUSED(isConnected)

    STVariable *variable = qobject_cast<STVariable*>(sender());
    emit variableConnectedStateChanged(variable->index());
}

void STVariableCollection::onVariableDescriptionChanged(const QString &description)
{
    Q_UNUSED(description)

    STVariable *variable = qobject_cast<STVariable*>(sender());
    emit variableDescriptionChanged(variable->index());
}

void STVariableCollection::onVariableUnitsChanged(const QString &units)
{
    Q_UNUSED(units)

    STVariable *variable = qobject_cast<STVariable*>(sender());
    emit variableUnitsChanged(variable->index());
}

void STVariableCollection::connectVariable(STVariable *toConnect)
{
    connect( toConnect, SIGNAL(connected(bool)), this, SLOT(onVariableConnectedStateChanged(bool)) );
    connect( toConnect, SIGNAL(descriptionChanged(QString)), this, SLOT(onVariableDescriptionChanged(QString)) );
    connect( toConnect, SIGNAL(unitsChanged(QString)), this, SLOT(onVariableUnitsChanged(QString)) );
}

void STVariableCollection::disconnectVariable(STVariable *variable)
{
    disconnect( variable, SIGNAL(connectedStateChanged(bool)), this, SLOT(onVariableConnectedStateChanged(bool)) );
    disconnect( variable, SIGNAL(descriptionChanged(QString)), this, SLOT(onVariableDescriptionChanged(QString)) );
    disconnect( variable, SIGNAL(unitsChanged(QString)), this, SLOT(onVariableUnitsChanged(QString)) );
}
