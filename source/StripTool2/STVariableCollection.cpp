#include "STVariableCollection.h"

STVariableCollection::STVariableCollection(QObject *parent) : QAbstractTableModel(parent)
{
    time_ = 0;
    timeFilteringEnabled_ = false;
    selection_ = 0;
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
        STVariable *toView = variables_.at(index.row());
        if (toView->hasDescription())
            return toView->description();
        else
            return toView->name();
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

QModelIndex STVariableCollection::indexOf(STVariable *toMatch) const
{
    return createIndex(variables_.indexOf(toMatch), 0);
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

QList<STVariable*> STVariableCollection::variablesWithSeries(MPlotItem *toMatch)
{
    QList<STVariable*> results;

    foreach (STVariable *variable, variables_) {
        if (variable->series() == toMatch)
            results.append(variable);
    }

    return results;
}

STVariable* STVariableCollection::selectedVariable() const
{
    return selection_;
}

void STVariableCollection::addVariable(const QString &name)
{
    if (name.isEmpty()) {
        return;
    }

    int newIndex = variables_.size();

    beginInsertRows(QModelIndex(), newIndex, newIndex);

    STVariable *newVariable = new STVariable(name, this);
    variables_.append(newVariable);

    newVariable->setIndex(newIndex);

    if (time_) {
        newVariable->setTimeValue(time_->value());
        newVariable->setTimeUnits(time_->units());
    }

    newVariable->setTimeFilteringEnabled(timeFilteringEnabled_);

    endInsertRows();
}

void STVariableCollection::deleteVariableAt(int indexToDelete)
{
    if (indexToDelete < 0 || indexToDelete >= variables_.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), indexToDelete, indexToDelete);
    STVariable *toDelete = variableAt(indexToDelete);
    variables_.removeAt(indexToDelete);
    toDelete->deleteLater();
    endRemoveRows();
}

void STVariableCollection::setTime(STTime *time)
{
    if (time_ != time && time != 0) {
        time_ = time;

        updateTimeValue(time_->value());
        updateTimeUnits(time_->units());

        connect( time_, SIGNAL(valueChanged(int)), this, SLOT(updateTimeValue(int)) );
        connect( time_, SIGNAL(unitsChanged(STTime::Units)), this, SLOT(updateTimeUnits(STTime::Units)) );
    }
}

void STVariableCollection::setTimeFilteringEnabled(bool isEnabled)
{
    if (timeFilteringEnabled_ != isEnabled) {
        timeFilteringEnabled_ = isEnabled;

        foreach (STVariable *variable, variables_) {
            variable->setTimeFilteringEnabled(timeFilteringEnabled_);
        }
    }
}

void STVariableCollection::setSelectedVariable(STVariable *newSelection)
{
    if (selection_) {
        selection_ = 0;
    }

    selection_ = newSelection;
    emit selectedVariableChanged(selection_);
}

void STVariableCollection::updateTimeValue(int newValue)
{
    foreach (STVariable *variable, variables_) {
        variable->setTimeValue(newValue);
    }
}

void STVariableCollection::updateTimeUnits(STTime::Units newUnits)
{
    foreach (STVariable *variable, variables_) {
        variable->setTimeUnits(newUnits);
    }
}
