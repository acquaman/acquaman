#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
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

    if (role == Qt::DisplayRole)
        return QVariant(pvList_.at(index.row())->pvDescription());

    if (role == Qt::ToolTipRole)
        return QVariant(pvList_.at(index.row())->pvName());

    if (role == Qt::CheckStateRole)
        return QVariant(pvList_.at(index.row())->checkState());

//    if (role == Qt::SizeHintRole)
//        return QVariant();

    else
        return QVariant();
}



bool StripToolModel::contains(const QString &nameToFind)
{
    bool itemFound = false;

    foreach (StripToolPV *pv, pvList_)
    {
        QString pvName = pv->pvName();
        if (pvName == nameToFind)
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



void StripToolModel::addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{
    int position = pvList_.size() - 1; // we insert new pvs at the end of the model.
    int count = 1; // insert one pv at a time.

    //  create new pv object and notify subscribers that it has been added to the model.
    StripToolPV *newPV = new StripToolPV(pvName, pvDescription, pvUnits);
    emit seriesChanged(Qt::Checked, newPV->series());   //  notify plot.

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify model view.

    pvList_.insert(position, newPV); // add the new pv to the model.

    endInsertRows();
}



void StripToolModel::deletePV(const QModelIndex &index)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        int position = index.row(); // we use the index to identify which pv to remove.
        int count = 1; // we delete pvs one at a time.

        //  identify pv entry to be deleted and notify subscribers that it will be removed.
        StripToolPV *toDelete = pvList_.at(position);
        emit seriesChanged(Qt::Unchecked, toDelete->series());  //  notify plot.

        beginRemoveRows(QModelIndex(), position, position + count - 1); //  notify model view.

        pvList_.removeAt(position); //  remove pv from model.
        delete toDelete;    // delete the pv object.

        endRemoveRows();
    }
}



void StripToolModel::setValuesDisplayed(const QModelIndex &index, int valuesDisplayed)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        connect( this, SIGNAL(updateValuesDisplayed(int)), toChange, SLOT(setValuesDisplayed(int)) );

        emit updateValuesDisplayed(valuesDisplayed);
        emit dataChanged(index, index);
    }
}



void StripToolModel::setPVUpdating(const QModelIndex &index, bool isUpdating)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(index.row());
        connect( this, SIGNAL(updateUpdating(bool)), toChange, SLOT(setPVUpdating(bool)) );

        emit updateUpdating(isUpdating);
        emit dataChanged(index, index);
    }
}



void StripToolModel::checkStateChanged(const QModelIndex &index, Qt::CheckState checked)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        //  1. edit pv information
        StripToolPV *toChange = pvList_.at(index.row());
        connect( this, SIGNAL(updateCheckState(Qt::CheckState)), toChange, SLOT(setCheckState(Qt::CheckState)));

        //  2. let subscribers know (pv, plot, view)
        emit updateCheckState(checked);
        emit seriesChanged(checked, toChange->series());
        emit dataChanged(index, index);
    }
}



void StripToolModel::descriptionChanged(const QModelIndex &index, const QString &newDescription)
{
    StripToolPV *toChange = pvList_.at(index.row());
    connect( this, SIGNAL(updateDescription(QString)), toChange, SLOT(setDescription(QString)) );

    emit updateDescription(newDescription);
    emit dataChanged(index, index);
}

