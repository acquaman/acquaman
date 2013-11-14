#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    appDirectory_ = 0;

    controlMapper_ = new QSignalMapper(this);
    connect( controlMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onPVConnected(QObject*)) );

    pvMapper_ = new QSignalMapper(this);
    connect( pvMapper_, SIGNAL(mapped(QObject*)), this, SLOT(toSavePVData(QObject*)) );

    connect( this, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
}



StripToolModel::~StripToolModel()
{
}



StripToolPV* StripToolModel::selectedPV() const
{
    return selectedPV_;
}



QModelIndex StripToolModel::selectedIndex() const
{
    return selectedIndex_;
}



MPlotItem* StripToolModel::selectedSeries() const
{
    if (selectedPV() == 0)
        return 0;
    else
        return selectedPV()->series();
}



MPlotItem* StripToolModel::series(int row) const
{
    MPlotItem *series = 0;

    if (row >= 0 && row < pvList_.size())
    {
        series = pvList_.at(row)->series();
    }

    return series;
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



bool StripToolModel::contains(StripToolPV *toMatch) const
{
    bool matchFound = false;
    int count = 0;

    while (count < pvList_.size() && !matchFound)
    {
        if (toMatch == pvList_.at(count))
            matchFound = true;

        count++;
    }

    return matchFound;
}



void StripToolModel::checkStateChanged(const QModelIndex &index, Qt::CheckState checked)
{
    int row = index.row();

    if (index.isValid() && row < pvList_.size())
    {
        StripToolPV *toChange = pvList_.at(row);
        toChange->setCheckState(checked);

        emit seriesChanged(checked, row);
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

        qDebug() << "\n\nAdding new pv...";
        AMReadOnlyPVControl *pvControl = new AMReadOnlyPVControl(pvName, pvName, this);
        controlMapper_->setMapping(pvControl, pvControl);

        connect( pvControl, SIGNAL(connected(bool)), controlMapper_, SLOT(map()) );
    }
}



void StripToolModel::onPVConnected(QObject* itemConnected)
{
    AMControl *pvControl = (AMControl*) itemConnected;

    if (pvControl->isConnected())
    {
        emit pvValid(true);
        qDebug() << "PV added : " << addPV(pvControl);

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
    pvMapper_->setMapping(newPV, newPV);

    connect( newPV, SIGNAL(savePV()), pvMapper_, SLOT(map()) );

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify list view and plot.
    pvList_.insert(position, newPV); // add new pv to the model.
    endInsertRows();

    setSelectedPV(newPV);

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



void StripToolModel::toSavePVData(QObject *toSave)
{
    StripToolPV *pvToSave = (StripToolPV *) toSave;
    QDir home, data;

    // check that the application directory exists.
    if (!appDirectory_.exists())
    {
        home = QDir(QDir::homePath());
        home.mkdir("StripTool");
        appDirectory_ = QDir(home.filePath("StripTool"));
    }

    // check that the location where the data for each pv is stored exists.
    QString dataPath = appDirectory_.absoluteFilePath("data");
    data = QDir(dataPath);

    if (!data.exists())
    {
        appDirectory_.mkdir("data");
    }

    appDirectory_.setCurrent(appDirectory_.filePath("data"));

    qDebug() << "Saving data for " << pvToSave->pvName() << " : " << savePVData(pvToSave);
}



bool StripToolModel::savePVData(StripToolPV *toSave)
{
    QString filename = toSave->pvName().replace(":", "_") + ".txt";

    if (filename.isEmpty())
        return false;

    QList<double> dataToSave = toSave->saveData().toList();
    QList<double> indexesToSave = toSave->saveIndexes().toList();

    QFile file(filename);

    if (!file.open(QIODevice::Append | QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
        return false;
    }

    QTextStream out(&file);

    for (int i = 0; i < dataToSave.size(); i++)
    {
        out << indexesToSave.at(i) << "\t" << dataToSave.at(i) << "\n";
    }

    file.close();

    return true;
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



void StripToolModel::setSelectedPV(StripToolPV *newSelection)
{
    if (newSelection)
    {
        if (contains(newSelection->pvName()))
        {
            selectedPV_ = newSelection;
            emit modelSelectionChange();
            qDebug() << "Selected pv : " << selectedPV_->pvName();

        } else {
            qDebug() << "ERROR : attempting to set selected pv to a pv not in the model.";
        }

    } else {
        selectedPV_ = 0;
        emit modelSelectionChange();
        qDebug() << "Deselected pv.";
    }
}



void StripToolModel::seriesSelected(MPlotItem *plotSelection)
{
    setSelectedPV(findItem(plotSelection));
}



void StripToolModel::seriesDeselected()
{
    setSelectedPV(0);
}



void StripToolModel::listItemSelected(const QModelIndex &index)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        setSelectedPV( pvList_.at(index.row()) );
    }
}



void StripToolModel::listItemDeselected()
{
    setSelectedPV(0);
}



void StripToolModel::onModelSelectionChange()
{
    if (selectedPV() == 0)
    {
        emit setPlotAxesLabels("", "");
        emit setPlotTicksVisible(false);

    } else {

        emit setPlotAxesLabels(selectedPV()->xUnits(), selectedPV()->yUnits());
        emit setPlotTicksVisible(true);
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
//    QFile file(pvFilename_);
//    if (file.exists())
//        emit showReloadDialog();
}

