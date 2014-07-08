/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "StripToolModel.h"

#include "StripChart/StripToolBasicVariableInfo.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    selectedVariable_ = 0;

    makeConnections();
    defaultSettings();

    qDebug() << "StripToolModel object created.";
}



StripToolModel::~StripToolModel()
{
}



StripToolVariable *StripToolModel::selectedVariable() const
{
    return selectedVariable_;
}



//MPlotItem* StripToolModel::plotItem(int row) const
//{
//    MPlotItem *series = 0;

//    if (row >= 0 && row < variables_.size())
//        series = variables_.at(row)->series();
////        series = variables_.at(row)->plotItem();

//    return series;
//}



Qt::ItemFlags StripToolModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;

}



QVariant StripToolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > variables_.size())
        return QVariant();

    StripToolVariable *toDisplay = variables_.at(index.row());

    if (role == Qt::DisplayRole) {
        return QVariant(toDisplay->info()->description());
    }

    if (role == Qt::ToolTipRole)
        return QVariant(toDisplay->info()->name());

    if (role == Qt::CheckStateRole)
        return QVariant(toDisplay->info()->checkState());

    return QVariant();
}



int StripToolModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return variables_.size();
}



bool StripToolModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    //  in the edit role, we expect the user wants to edit the pv description.
    if (role == Qt::EditRole) {
        onDescriptionChanged(index, value.toString());
        return true;
    }

    // in the check state role, we expect the user wants to change a pv's visibility.
    if (role == Qt::CheckStateRole) {
        onCheckStateChanged(index, static_cast<Qt::CheckState>(value.toInt()));
        return true;
    }

    return false;
}



bool StripToolModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}



bool StripToolModel::contains(const QString &nameToFind) const
{
    if (findVariable(nameToFind) == 0)
        return false;
    else
        return true;
}



StripToolVariable *StripToolModel::findVariable(const QString &pvName) const
{
    StripToolVariable *match = 0;
    bool found = false;
    int count = 0;

    while(count < variables_.size() && !found) {
        if (pvName == variables_.at(count)->info()->name()) {
            match = variables_.at(count);
            found = true;
        }

        count++;
    }

    return match;
}



bool StripToolModel::contains(const QModelIndex &indexToFind) const {
    if (findVariable(indexToFind) == 0)
        return false;
    else
        return true;
}


StripToolVariable* StripToolModel::findVariable(const QModelIndex &index) const {
    if (index == QModelIndex())
        return 0;

    if (index.row() >= variables_.size())
        return 0;

    if (index.row() < 0)
        return 0;

    else
        return variables_.at(index.row());
}



bool StripToolModel::contains(MPlotItem *itemToFind) const {
    if (findVariable(itemToFind) == 0)
        return false;
    else
        return true;
}



StripToolVariable* StripToolModel::findVariable(MPlotItem *plotItem) const
{
    StripToolVariable *match = 0;

    if (plotItem) {
        bool matchFound = false;
        int variableIndex = 0;
        int variableNumber = variables_.length();

        while (variableIndex < variableNumber && !matchFound) {
            StripToolVariable *variable = variables_.at(variableIndex);

            if (variable->series() == plotItem) {
                matchFound = true;
                match = variable;
            }

            variableIndex++;
        }

        if (!matchFound)
            qDebug() << "StripToolModel::findVariable(MPlotItem*) : no match found.";
    }

    return match;
}



StripToolVariable* StripToolModel::findVariable(int row) const
{
    StripToolVariable* match = 0;
    bool matchFound = false;

    if (row >= 0 && row < variables_.length()) {
        match = variables_.at(row);
        matchFound = true;
    }


    if (!matchFound)
        qDebug() << "StripToolModel::findVariable(int) : no match found.";

    return match;
}



bool StripToolModel::contains(StripToolVariable *toMatch) const
{
    bool matchFound = false;
    int count = 0;

    while (count < variables_.size() && !matchFound) {
        if (toMatch == variables_.at(count))
            matchFound = true;

        count++;
    }

    return matchFound;
}



void StripToolModel::onCheckStateChanged(const QModelIndex &index, Qt::CheckState checked)
{
    if (index.isValid() && index.row() < variables_.size()) {
        StripToolVariable *toChange = variables_.at(index.row());
        toChange->info()->setCheckState(checked);

        emit variableCheckStateChanged(index);
        emit dataChanged(index, index);
    }
}



void StripToolModel::onDescriptionChanged(const QModelIndex &index, const QString &newDescription)
{
    StripToolVariable *toChange = variables_.at(index.row());
    toChange->info()->setDescription(newDescription);
    emit dataChanged(index, index);
}



bool StripToolModel::addVariable(StripToolVariable* variable)
{
    qDebug() << "StripToolModel :: Adding new variable" << variable->info()->name() << "to the model.";

    int position = variables_.size(); // append new pvs to the end of the model.
    int count = 1; // insert one pv at a time.

    variable->setParent(this);
    variable->setIndex(createIndex(position, 0));

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify list view and plot.
    variables_.insert(position, variable); // add new pv to the model.
    endInsertRows();

    qDebug() << "StripToolModel :: new variable added to model.";

    if (position + count == variables_.size()) {
        setSelectedVariable(variable);
        qDebug() << "StripToolModel :: successfully added new variable to model.";
        return true;

    } else {

        setSelectedVariable(0);
        qDebug() << "Failed to add pv -- unknown cause.";
        return false;
    }
}



bool StripToolModel::addVariable(StripToolVariableInfo *info) {
    qDebug() << "StripToolModel :: adding variable from info.";
    return addVariable(new StripToolVariable(info, this));
}



bool StripToolModel::removeVariable(const QModelIndex &index)
{
    bool successful = false;

    if (index.isValid() && contains(index)) {
        StripToolVariable *toRemove = findVariable(index);

        if (selectedVariable_ == toRemove)
            setSelectedVariable(0);

        int position = index.row();
        int count = 1; // we delete one pv at a time.
        int varNum = variables_.size();


        beginRemoveRows(QModelIndex(), position, position + count - 1);
        variables_.removeAll(toRemove);
        endRemoveRows();

        if (varNum - 1 == variables_.size()) {
            qDebug() << "StripToolModel :: successfully removed variable from model.";
            successful = true;

        } else {
            qDebug() << "StripToolModel :: remove variable failed.";
            successful = false;
        }

    } else {
        qDebug() << "StripToolModel :: remove variable failed : could not identify valid variable to remove.";
        successful = false;
    }

    return successful;
}



bool StripToolModel::deleteVariable(StripToolVariable *variable)
{
    Q_UNUSED(variable)
    qDebug() << "Deleting variable and all data... when implemented!";
    return false;
}



void StripToolModel::setSelectedVariable(StripToolVariable *newSelection)
{
    if (selectedVariable_ != newSelection) {

        if (selectedVariable_ != 0) {
            deselectSelectedVariable();
            qDebug() << "StripToolModel :: variable deselected.";
        }

        if (newSelection != 0 && contains(newSelection)) {
            selectVariable(newSelection);
            qDebug() << "StripToolModel :: variable selected : " << selectedVariable_->info()->name();
        }

        emit modelSelectionChange();
    }
}



//void StripToolModel::enableWaterfall(bool isEnabled)
//{
//    foreach (StripToolVariable *variable, variables_) {
//        variable->series()->enableWaterfall(isEnabled, variable->index().row(), variables_.size());
//    }
//}



void StripToolModel::changeDisplayedTimeAmount(int amount)
{
    foreach (StripToolVariable *variable, variables_) {
        variable->info()->setTimeAmount(amount);
    }
}



void StripToolModel::changeDisplayedTimeUnits(TimeEntryWidget::TimeUnits units)
{
    foreach (StripToolVariable *variable, variables_) {
        variable->info()->setTimeUnits(units);
    }
}



void StripToolModel::deselectSelectedVariable()
{
    if (selectedVariable_) {
        disconnectSelectedVariable();
        selectedVariable_->info()->setSelectionState(false);
        selectedVariable_ = 0;
    }
}



void StripToolModel::disconnectSelectedVariable()
{
    if (selectedVariable_ != 0) {

        disconnect( selectedVariable_->info(), SIGNAL(infoChanged()), this, SIGNAL(selectedVariableInfoChanged()) );
        disconnect( selectedVariable_, SIGNAL(displayRangeUpdated(const MPlotAxisRange*)), this, SIGNAL(selectedVariableDisplayRangeChanged(const MPlotAxisRange*)) );
    }
}



void StripToolModel::selectVariable(StripToolVariable *newSelection)
{
    if (selectedVariable_ != 0) {
        deselectSelectedVariable();
    }

    if (contains(newSelection)) {
        selectedVariable_ = newSelection;
        selectedVariable_->info()->setSelectionState(true);
        connectSelectedVariable();

    } else {
        qDebug() << "StripToolModel :: cannot select a variable that isn't in the model. Item deselected.";
    }
}


void StripToolModel::connectSelectedVariable()
{
    if (selectedVariable_ != 0) {
        connect( selectedVariable_->info(), SIGNAL(infoChanged()), this, SIGNAL(selectedVariableInfoChanged()) );
        connect( selectedVariable_, SIGNAL(displayRangeUpdated(const MPlotAxisRange*)), this, SIGNAL(selectedVariableDisplayRangeChanged(const MPlotAxisRange*)) );
    }
}



//void StripToolModel::colorPV(const QModelIndex &index, const QColor &color)
//{
//    if (index.isValid() && index.row() < variables_.size()) {
//        StripToolVariable *toEdit = variables_.at(index.row());
//        toEdit->info()->setColor(color);
//    }
//}



void StripToolModel::toTestRangeSignal(MPlotAxisRange *newRange)
{
    qDebug() << "StripToolModel :: the model received a new signal range with min ->" << newRange->min() << "and max ->" << newRange->max();
}



void StripToolModel::makeConnections()
{
}



void StripToolModel::defaultSettings()
{
}

