#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
//    updateNumber_ = 0;

    // when the pv control signals that it has successfully connected to EPICS, we know the pv is valid and can proceed to add it.
    controlMapper_ = new QSignalMapper(this);
    connect( controlMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onPVConnected(QObject*)) );

    // used when the StripToolPV signals that it is ready to save its collected data.
    saveDataMapper_ = new QSignalMapper(this);
    connect( saveDataMapper_, SIGNAL(mapped(QObject*)), this, SIGNAL(savePVData(QObject*)) );

    // used when the StripToolPV signals that it is ready for changes to its metadata to be saved.
    saveMetadataMapper_ = new QSignalMapper(this);
    connect( saveMetadataMapper_, SIGNAL(mapped(QObject*)), this, SIGNAL(savePVMetadata(QObject*)) );

    pvUpdatedMapper_ = new QSignalMapper(this);
    connect( pvUpdatedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSinglePVUpdated(QObject*)) );

    connect( this, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
    connect( this, SIGNAL(pvUpdating(QModelIndex,bool)), this, SLOT(setPVUpdating(QModelIndex,bool)) );
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
    if (selectedPV_ == 0)
        return QModelIndex();

    else
        return createIndex(pvList_.indexOf(selectedPV_), 0);
}



MPlotItem* StripToolModel::selectedSeries() const
{
    if (selectedPV_ == 0)
        return 0;
    else
        return selectedPV_->series();
}



MPlotItem* StripToolModel::series(int row) const
{
    MPlotItem *series = 0;

    if (row >= 0 && row < pvList_.size())
        series = pvList_.at(row)->series();

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



StripToolPV* StripToolModel::findItem(const QString &pvName) const
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



StripToolPV* StripToolModel::findItem(MPlotItem *series) const
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



bool StripToolModel::contains(const QString &nameToFind) const
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
    qDebug() << "Pv to add : " << pvName;

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

    if (position == 0)
        modelStartTime_ = QTime::currentTime();

    StripToolPV *newPV = new StripToolPV(modelStartTime_, this);
    newPV->setControl(pvControl);
    saveDataMapper_->setMapping(newPV, newPV);
    saveMetadataMapper_->setMapping(newPV, newPV);
    pvUpdatedMapper_->setMapping(newPV, newPV);

    connect( newPV, SIGNAL(savePVData()), saveDataMapper_, SLOT(map()) );
    connect( newPV, SIGNAL(savePVMetaData()), saveMetadataMapper_, SLOT(map()) );
    connect( newPV, SIGNAL(pvValueUpdated()), pvUpdatedMapper_, SLOT(map()) );

    connect( this, SIGNAL(forceUpdatePVs(QString)), newPV, SLOT(toForceUpdateValue(QString)) );

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify list view and plot.
    pvList_.insert(position, newPV); // add new pv to the model.
    endInsertRows();

    if (position + 1 == pvList_.size())
    {
        setSelectedPV(newPV);
        qDebug() << "Requesting meta data for pv" << newPV->pvName() << "if it exists...";
        emit metaDataCheck(newPV->pvName());
        return true;

    } else {

        qDebug() << "Failed to add pv -- unknown cause.";
        return false;
    }
}



void StripToolModel::editPV(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= pvList_.size())
    {
        qDebug() << "This index is either invalid or refers to a value outside the list of current pvs.";
        return;
    }

    QList<QString> pvInfo = pvList_.at(index.row())->metaData();

    EditPVDialog editDialog(pvInfo);
    if (editDialog.exec())
    {
        QString description = editDialog.description();
        QString units = editDialog.units();
        int points = editDialog.points();

        StripToolPV *toEdit = pvList_.at(index.row());

        if (description != "")
        {
            toEdit->setDescription(description);
            emit dataChanged(index, index);
        }

        if (units != "")
            toEdit->setUnits(units);

        if (points > 0)
            toEdit->setValuesDisplayed(points);

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

        setSelectedPV(0);
        emit deletePVData(pvList_.at(index.row()));
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



void StripToolModel::toPausePVs()
{
    int pvCount = pvList_.size();

    for (int i = 0; i < pvCount; i++)
    {
        emit pvUpdating(createIndex(i, 0), false);
    }
}



void StripToolModel::toResumePVs()
{
    int pvCount = pvList_.size();

    for (int i = 0; i < pvCount; i++)
    {
        emit pvUpdating(createIndex(i, 0), true);
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



void StripToolModel::setSelectedPV(StripToolPV *newSelection)
{
    if (newSelection != selectedPV_)
    {
        if (newSelection && contains(newSelection))
        {
            qDebug() << "Setting selected pv...";
            selectedPV_ = newSelection;
            emit modelSelectionChange();
            qDebug() << "Selected pv : " << selectedPV_->pvName();

        } else if (!newSelection) {

            selectedPV_ = 0;
            emit modelSelectionChange();
            qDebug() << "Deselected pv.";

        } else {

            selectedPV_ = 0;
            emit modelSelectionChange();
            qDebug() << "Attempting to set an unknown pv as selected!!";
        }
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



void StripToolModel::listItemSelected(const QModelIndex &newSelection, const QModelIndex &oldSelection)
{
    Q_UNUSED(oldSelection);

    if (newSelection == QModelIndex())
        setSelectedPV(0);

    else if (newSelection.isValid() && newSelection.row() < pvList_.size())
        setSelectedPV( pvList_.at(newSelection.row()) );
}



void StripToolModel::onModelSelectionChange()
{
    if (selectedPV_ == 0)
    {
        emit setPlotAxesLabels("", "");
        emit setPlotTicksVisible(false);

    } else {

        emit setPlotAxesLabels(selectedPV()->xUnits(), selectedPV()->yUnits());
        emit setPlotTicksVisible(true);
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



void StripToolModel::toSetMetaData(const QString &pvName, QList<QString> metaData)
{
    qDebug() << "Meta data found. Attempting to find matching pv...";
    StripToolPV *toEdit = findItem(pvName);

    if (toEdit != 0)
        qDebug() << "Editing metadata for pv" << pvName << ":" << toEdit->setMetaData(metaData);

    else
        qDebug() << "Matching pv not found.";
}



void StripToolModel::onSinglePVUpdated(QObject *pvUpdated)
{
//    updateNumber_++;

    StripToolPV *updated = (StripToolPV *) pvUpdated;
    emit forceUpdatePVs(updated->pvName());
}

