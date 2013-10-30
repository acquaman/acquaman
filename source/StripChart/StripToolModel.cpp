#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    connect( this, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
    connect( this, SIGNAL(updateActivePVs()), this, SLOT(saveActivePVs()) );
}



StripToolModel::~StripToolModel()
{
}



void StripToolModel::setSaveDirectory(QDir &newDir)
{
    saveDirectory_ = newDir;
}



void StripToolModel::setPVFilename(const QString &pvFilename)
{
    pvFilename_ = pvFilename;
}



Qt::ItemFlags StripToolModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled;

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



QVariant StripToolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( (section > pvList_.size()) || (section < 0) )
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    else
        return QVariant(headerData_.at(section));
}



int StripToolModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pvList_.size();
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



bool StripToolModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (section != 1)
        return false;

    if (orientation != Qt::Horizontal)
        return false;

    if (role != Qt::EditRole)
        return false;

    if (value.isNull())
        return false;

    else {
        headerData_[section] = value;
        emit headerDataChanged(orientation, section, section);
        return true;
    }
}



bool StripToolModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}



bool StripToolModel::contains(const QString &nameToFind)
{    
    bool found = false;
    int count = 0;

    while(!found && count < pvList_.size())
    {
        if(pvList_.at(count)->pvName() == nameToFind)
            found = true;

        count++;
    }

    return found;
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

    emit updateActivePVs(); // update the information on active pvs that's saved to file.
}



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

                if (newPoints > 0)
                    toEdit->setValuesDisplayed(newPoints);
            }

            emit updateActivePVs();
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

        beginRemoveRows(QModelIndex(), position, position + count - 1); //  notify model view.
        delete pvList_.takeAt(position); //remove pv from model and delete.
        endRemoveRows();

        emit seriesChanged(Qt::Unchecked, toDelete->series());  //  notify plot.

        emit updateActivePVs();
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



void StripToolModel::savePV(const QModelIndex &index)
{
    Q_UNUSED(index);

//    if (index.isValid() && index.row() < pvList_.size())
//    {
//        StripToolPV *toSave = pvList_.at(index.row());
//        QVector<double> dataToSave = toSave->saveData();

//        QString filename = saveDirectory_.filePath(toSave->pvName() + ".txt");

//        QFile file(filename);

//        if (!file.open(QIODevice::WriteOnly))
//        {
//            qDebug() << file.errorString();
//            return;
//        }

//        QDataStream out(&file);
//        out.setVersion(QDataStream::Qt_4_5);
//        out << dataToSave;
//    }
}



void StripToolModel::seriesSelected(MPlotItem *plotSelection, bool isSelected)
{
    if (!isSelected)
        selectedPV_ = 0;

    else {

        for(int i = 0; i < pvList_.size(); i++)
        {
            StripToolPV *pv = pvList_.at(i);

            if (pv->series() == plotSelection)
            {
                selectedIndex_ = createIndex(i, 1, 0);
                selectedPV_ = pv;
                break;
            }
        }
    }

    emit modelSelectionChange();
}



void StripToolModel::itemSelected(const QModelIndex &index)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        selectedIndex_ = index;
        selectedPV_ = pvList_.at(selectedIndex_.row());
        emit modelSelectionChange();
    }
}



void StripToolModel::onModelSelectionChange()
{
    if (selectedPV_ == 0)
    {
        emit setPlotAxesLabels("", "");
        qDebug() << "Selected PV : none";

    } else {

        //  first, tell the plot what it should now be displaying.
        emit setPlotAxesLabels(selectedPV_->xUnits(), selectedPV_->yUnits());
        emit setSeriesSelected(selectedPV_->series());

        qDebug() << "Selected PV : " + selectedPV_->pvName();

        //  next, communicate the change to the list view.
        emit setItemSelected(selectedIndex_);
    }
}



void StripToolModel::saveActivePVs()
{
    //  write the updated collection to file.
    QFile file(pvFilename_);

    if (!file.open(QIODevice::WriteOnly))
        qDebug() << pvFilename_ + " : " + file.errorString();

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_5);

    QList<QStringList> pvs;

    foreach(StripToolPV *pv, pvList_)
    {
        QStringList pvEntry;
        pvEntry << pv->pvName() << pv->pvDescription() << pv->yUnits();
        pvs << pvEntry;
    }

    out << pvs;
}



void StripToolModel::reloadPVs(bool reload)
{
    QFile file(pvFilename_);

    if (reload && file.exists())
    {
        qDebug() << "reloading pvs...";

        if (!file.open(QIODevice::ReadOnly))
            qDebug() << pvFilename_ + " : " + file.errorString();

        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_5);

        QList<QStringList> pvs;
        in >> pvs;

        qDebug() << pvs;

        foreach(QStringList pvEntry, pvs)
        {
            emit addPV(pvEntry.at(0), pvEntry.at(1), pvEntry.at(2));
        }

    } else {

        if (file.exists())
            file.remove();
    }
}



void StripToolModel::colorPV(const QModelIndex &index, const QColor &color)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toEdit = pvList_.at(index.row());
        toEdit->setSeriesColor(color);
    }
}

