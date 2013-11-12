#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent, QDir saveDirectory) : QAbstractListModel(parent)
{
    saveDirectory_ = saveDirectory;
    pvFilename_ = "addedPVs.txt";

    mapper_ = new QSignalMapper(this);
    connect( mapper_, SIGNAL(mapped(QObject*)), this, SLOT(onPVConnected(QObject*)) );

    connect( this, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
}



StripToolModel::~StripToolModel()
{
}



QModelIndex StripToolModel::selectedIndex() const
{
    return selectedIndex_;
}



MPlotItem* StripToolModel::selectedSeries()
{
    return selectedPV_->series();
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
    if (role == Qt::EditRole && value.toString() != "")
    {
        descriptionChanged(index, value.toString());
        return true;
    }

    // in the check state role, we expect the user wants to change a pv's visibility.
    if (role == Qt::CheckStateRole)
    {
        checkStateChanged(index, static_cast<Qt::CheckState>(value.toInt()));
        return true;
    }

    return false;
}



bool StripToolModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}



StripToolPV* StripToolModel::findItem(const QString &pvName)
{
    StripToolPV *match = 0;
    bool found = false;
    int count = 0;

    while(count < pvList_.size() && !found)
    {
        if (pvName == pvList_.at(count)->pvName())
        {
            match = pvList_.at(count);
            found = true;
        }

        count++;
    }

    return match;
}



StripToolPV* StripToolModel::findItem(MPlotItem *series)
{
    StripToolPV *match = 0;
    bool found = false;
    int count = 0;

    while (count < pvList_.size() && !found)
    {
        if (series == pvList_.at(count)->series())
        {
            match = pvList_.at(count);
            found = true;
        }

        count++;
    }

    return match;
}



bool StripToolModel::contains(const QString &nameToFind)
{    
    if (findItem(nameToFind) == 0)
        return false;
    else
        return true;
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



void StripToolModel::toAddPV(const QString &pvName)
{

    if (pvName == "") {
        emit errorMessage("PV name is empty.");
        emit pvValid(false);

    } else if (contains(pvName)) {
        emit errorMessage("PV already added.");
        emit pvValid(false);

    } else {

        AMReadOnlyPVControl *pvControl = new AMReadOnlyPVControl(pvName, pvName, this);
        mapper_->setMapping(pvControl, pvControl);

        connect( pvControl, SIGNAL(connected(bool)), mapper_, SLOT(map()) );
    }
}



void StripToolModel::onPVConnected(QObject* itemConnected)
{
    AMControl *pvControl = (AMControl*) itemConnected;

    if (pvControl->isConnected())
    {
        emit pvValid(true);
        qDebug() << "Adding new pv : " << addPV(pvControl);

    } else {
        emit errorMessage("Invalid pv name.");
        emit pvValid(false);
        pvControl->deleteLater();
    }
}



bool StripToolModel::addPV(AMControl *pvControl)
{
    int position = pvList_.size(); // append new pvs to the end of the model.
    int count = 1; // insert one pv at a time.

    StripToolPV *newPV = new StripToolPV(pvControl->name(), "", "", this);
    newPV->setControl(pvControl);

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify model view.
    pvList_.insert(position, newPV); // add new pv to the model.
    endInsertRows();

    emit seriesChanged(Qt::Checked, newPV->series());   //  notify plot.

    selectedPV_ = newPV;
    emit modelSelectionChange();

    if (position + 1 == pvList_.size())
    {
        return true;

    } else {

        emit errorMessage("PV unsuccessfully added--unknown cause.");
        return false;
    }
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
        }
    }
}



void StripToolModel::toDeletePV(const QModelIndex &index)
{
    qDebug() << "Deleting pv : " << deletePV(index);
}



bool StripToolModel::deletePV(const QModelIndex &index)
{
    int size = pvList_.size();

    if (index.isValid() && index.row() < size)
    {
        int position = index.row(); //  we use the index to identify which pv to remove from the list.
        int count = 1; //  we delete pvs one at a time.

        selectedPV_ = 0;
        emit modelSelectionChange();

        emit seriesChanged(Qt::Unchecked, pvList_.at(position)->series());  //  notify plot.

        pvList_.at(position)->disconnect(this);

        beginRemoveRows(QModelIndex(), position, position + count - 1); //  notify model view.
        pvList_.takeAt(position)->deleteLater(); //  remove pv from model.
        endRemoveRows();

        if (size == pvList_.size() + 1)
        {
            return true;

        } else {

            emit errorMessage("PV deleted unsuccessfully--unknown cause.");
            return false;
        }

    } else {

        emit errorMessage("Cannot delete pv at invalid index.");
        return false;
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



void StripToolModel::toChangeModelSelection(MPlotItem* item, bool isSelected)
{
    if (isSelected)
        selectedPV_ = findItem(item);
    else
        selectedPV_ = 0;

    emit modelSelectionChange();
}



void StripToolModel::toChangeModelSelection(const QModelIndex &index, bool isSelected)
{
    if (isSelected)
        selectedPV_ = pvList_.at(index.row());
    else
        selectedPV_ = 0;

    emit modelSelectionChange();
}



void StripToolModel::onModelSelectionChange()
{
    if (selectedPV_ == 0)
    {
        emit setPlotAxesLabels("", "");
        emit setPlotTicksVisible(false);
        qDebug() << "Selected PV : " << 0;

    } else {

        //  first, tell the plot what it should now be displaying.
        emit setPlotAxesLabels(selectedPV_->xUnits(), selectedPV_->yUnits());
        emit setPlotTicksVisible(true);

        qDebug() << "Selected PV : " + selectedPV_->pvName();
    }
}



void StripToolModel::toUpdatePVFile()
{
//    //  write the updated collection to file.
//    QFile file(pvFilename_);

//    if (!file.open(QIODevice::WriteOnly))
//        qDebug() << pvFilename_ + " : " + file.errorString();

//    QDataStream out(&file);
//    out.setVersion(QDataStream::Qt_4_5);

//    QList<QStringList> pvs;

//    foreach(StripToolPV *pv, pvList_)
//    {
//        QStringList pvEntry;
//        pvEntry << pv->pvName() << pv->pvDescription() << pv->yUnits();
//        pvs << pvEntry;
//    }

//    out << pvs;
}



void StripToolModel::reloadPVs(bool reload)
{
    Q_UNUSED(reload);
//    if (reload == true)
//    {
//        qDebug() << "reloading pvs...";

//        QFile file(pvFilename_);

//        if (!file.open(QIODevice::ReadOnly))
//            qDebug() << pvFilename_ + " : " + file.errorString();

//        QDataStream in(&file);
//        in.setVersion(QDataStream::Qt_4_5);

//        QList<QStringList> pvs;
//        in >> pvs;

//        qDebug() << pvs;

//        foreach(QStringList pvEntry, pvs)
//        {
//            addPV(0, pvEntry.at(0), pvEntry.at(1), pvEntry.at(2));
//        }

//    } else {

//    }
}



void StripToolModel::colorPV(const QModelIndex &index, const QColor &color)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toEdit = pvList_.at(index.row());
        toEdit->setSeriesColor(color);
    }
}



void StripToolModel::reloadCheck()
{
    QFile file(pvFilename_);
    if (file.exists())
        emit showReloadDialog();
}

