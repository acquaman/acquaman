#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    connect( this, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
}



StripToolModel::~StripToolModel()
{
}



int StripToolModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pvList_.size();
}



QVariant StripToolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > pvList_.size())
        return QVariant();

    StripToolPV *toDisplay = pvList_.at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (toDisplay->pvDescription() == "")
            return QVariant(toDisplay->pvName());
        else
            return QVariant(toDisplay->pvDescription());
    }

    if (role == Qt::ToolTipRole)
        return QVariant(toDisplay->pvName());

    if (role == Qt::CheckStateRole)
        return QVariant(toDisplay->checkState());

    return QVariant();
}



bool StripToolModel::contains(const QString &nameToFind)
{
    bool itemFound = false;

    foreach (StripToolPV *pv, pvList_)
    {
        if (pv->pvName() == nameToFind)
            itemFound = true;
    }

    return itemFound;
}



bool StripToolModel::insertRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), position, position + count - 1);

    for (int row = 0; row < count; ++row)
    {
        pvList_.insert(position, 0);
    }

    endInsertRows();
    return true;
}



bool StripToolModel::removeRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), position, position + count - 1);

    for (int row = 0; row < count; ++row)
    {
        pvList_.removeAt(position);
    }

    endRemoveRows();
    return true;
}



Qt::ItemFlags StripToolModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled;

}



bool StripToolModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    //  in the edit role, we expect the user wants to edit the pv description.
    if (role == Qt::EditRole)
    {
        if (value.toString() == "")
        {
            return false;

        } else {
            descriptionChanged(index, value.toString());
            return true;
        }
    }

    // in the check state role, we expect the user wants to change a pv's visibility.
    if (role == Qt::CheckStateRole)
    {
        checkStateChanged(index, static_cast<Qt::CheckState>(value.toInt()));
    }

    return false;
}



void StripToolModel::checkStateChanged(const QModelIndex &index, Qt::CheckState checked)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        toChange->setCheckState(checked);

        emit seriesChanged(checked, toChange->series());
        emit dataChanged(index, index);
    }
}



void StripToolModel::descriptionChanged(const QModelIndex &index, const QString &newDescription)
{
    StripToolPV *toChange = pvList_.at(index.row());
    toChange->setDescription(newDescription);
    emit dataChanged(index, index);
}



void StripToolModel::addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{
    int position = pvList_.size() - 1; // we insert new pvs at the end of the model.
    int count = 1; // insert one pv at a time.

    //  create new pv object and notify views.
    StripToolPV *newPV = new StripToolPV(pvName, pvDescription, pvUnits, this);

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify model view.
    pvList_.insert(position, newPV); // add the new pv to the model.
    endInsertRows();

    emit seriesChanged(Qt::Checked, newPV->series());   //  notify plot.
}



//void StripToolModel::editPV(const QModelIndex &index)
//{
//    if (index.isValid() && index.row() < pvList_.size())
//    {
//        StripToolPV *toEdit = pvList_.at(index.row());
//        EditPVDialog editDialog(toEdit->pvName());

//        if (editDialog.exec())
//        {
//            QString newDescription = editDialog.description();
//            QString newUnits = editDialog.units();
//            int newPoints = editDialog.points();

//            toEdit->setDescription(newDescription);
//            toEdit->setUnits(newUnits);
//            toEdit->setValuesDisplayed(newPoints);

//            emit dataChanged(index, index);
//        }
//    }
//}



void StripToolModel::editPV(QList<QModelIndex> indicesToEdit)
{
    QList<QModelIndex> okIndices;
    QStringList okNames;

    foreach(const QModelIndex &index, indicesToEdit)
    {
        if (index.isValid() && index.row() < pvList_.size())
        {
            okIndices.append(index);
            okNames << data(index, Qt::DisplayRole).toString();
        }
    }

    if (okNames.length() > 0)
    {
        EditPVDialog editDialog(okNames);
        if (editDialog.exec())
        {
            QString newDescription = editDialog.description();
            QString newUnits = editDialog.units();
            int newPoints = editDialog.points();

            foreach(const QModelIndex &index, okIndices)
            {
                StripToolPV *toEdit = pvList_.at(index.row());

                if (newDescription != "")
                {
                    toEdit->setDescription(newDescription);
                    emit dataChanged(index, index);
                }

                if (newUnits != "")
                    toEdit->setUnits(newUnits);

                if (newPoints >= 0)
                    toEdit->setValuesDisplayed(newPoints);
            }
        }
    }
}



void StripToolModel::deletePV(const QModelIndex &index)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        int position = index.row(); // we use the index to identify which pv to remove.
        int count = 1; // we delete pvs one at a time.

        StripToolPV *toDelete = pvList_.at(position);

        emit seriesChanged(Qt::Unchecked, toDelete->series());  //  notify plot.

        beginRemoveRows(QModelIndex(), position, position + count - 1); //  notify model view.
        pvList_.removeAt(position); //  remove pv from model.
        endRemoveRows();
    }
}



void StripToolModel::setPVUpdating(const QModelIndex &index, bool isUpdating)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        toChange->setPVUpdating(isUpdating);
    }
}



void StripToolModel::setValuesDisplayed(const QModelIndex &index, int points)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        toChange->setValuesDisplayed(points);
    }
}



void StripToolModel::incrementValuesDisplayed(const QModelIndex &index, int difference)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        toChange->incrementValuesDisplayed(difference);
    }
}



//void StripToolModel::setAllValuesDisplayed(const QModelIndex &index)
//{
//    if (index.isValid() && index.row() < pvList_.size())
//    {
//        StripToolPV *toChange = pvList_.at(index.row());
//        toChange->setAllValuesDisplayed(true);
//    }
//}



void StripToolModel::seriesSelected(MPlotItem *plotSelection, bool isSelected)
{
    if (!isSelected)
    {
        selectedPV_ = 0;

    } else {

        foreach(StripToolPV *pv, pvList_)
        {
            if (pv->series() == plotSelection)
                selectedPV_ = pv;
        }
    }

    emit modelSelectionChange();
}



void StripToolModel::onModelSelectionChange()
{
    if (selectedPV_ == 0)
    {
        emit setPlotAxesLabels("", "");

    } else {

        //  first, tell the plot what it should now be displaying.
        emit setPlotAxesLabels(selectedPV_->xUnits(), selectedPV_->yUnits());

        //  next, communicate the change to the list view.
    }
}

