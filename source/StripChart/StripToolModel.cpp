#include "StripToolModel.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    selectedPV_ = 0;

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


QString StripToolModel::selectedDescription() const
{
    if (selectedPV_ == 0)
        return 0;
    else
        return selectedPV_->pvDescription();
}


QString StripToolModel::selectedUnits() const
{
    if (selectedPV_ == 0)
        return 0;
    else
        return selectedPV_->yUnits();
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
        QMessageBox errorMsg;
        errorMsg.setText("PV name cannot be blank.");
        errorMsg.exec();
        emit pvValid(false);

    } else if (contains(pvName)) {
        QMessageBox errorMsg;
        errorMsg.setText("PV with that name already added.");
        errorMsg.exec();
        emit pvValid(false);

    } else {

        qDebug() << "Adding new pv...";
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
        QMessageBox errorMsg;
        errorMsg.setText("Invalid pv name.");
        errorMsg.exec();

        emit pvValid(false);
        pvControl->deleteLater();
    }
}



bool StripToolModel::addPV(AMControl *pvControl)
{
    int position = pvList_.size(); // append new pvs to the end of the model.
    int count = 1; // insert one pv at a time.

    StripToolPV *newPV = new StripToolPV(this);
    newPV->setControl(pvControl);
    saveDataMapper_->setMapping(newPV, newPV);
    saveMetadataMapper_->setMapping(newPV, newPV);
    pvUpdatedMapper_->setMapping(newPV, newPV);

    connect( newPV, SIGNAL(savePVData()), saveDataMapper_, SLOT(map()) );
    connect( newPV, SIGNAL(savePVMetaData()), saveMetadataMapper_, SLOT(map()) );
    connect( newPV, SIGNAL(pvValueUpdated()), pvUpdatedMapper_, SLOT(map()) );
    connect( newPV, SIGNAL(updateYAxisRange(MPlotAxisRange *)), this, SIGNAL(updateYAxisRange(MPlotAxisRange *)) );
    connect( newPV, SIGNAL(updateYAxisLabel(QString)), this, SIGNAL(updateYAxisLabel(QString)) );
    connect( newPV, SIGNAL(updateWaterfall(double)), this, SIGNAL(updateWaterfall(double)) );

    connect( this, SIGNAL(forceUpdatePVs(QString)), newPV, SLOT(toForceUpdateValue(QString)) );
    connect( this, SIGNAL(updateTime(int)), newPV, SLOT(toUpdateTime(int)) );
    connect( this, SIGNAL(updateTimeUnits(QString)), newPV, SLOT(toUpdateTimeUnits(QString)) );

    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify list view and plot.
    pvList_.insert(position, newPV); // add new pv to the model.
    endInsertRows();

    if (position + count == pvList_.size()) {
        setSelectedPV(newPV);

        qDebug() << "Requesting meta data for pv" << newPV->pvName() << "if it exists...";
        emit metaDataCheck(newPV->pvName());

        qDebug() << "Requesting time update.";
        emit requestTimeUpdate();

        return true;

    } else {

        setSelectedPV(0);
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
        int granularity = editDialog.granularity();

        StripToolPV *toEdit = pvList_.at(index.row());

        if (description != "")
        {
            toEdit->setDescription(description);
            emit dataChanged(index, index);
        }

        if (units != "")
            toEdit->setYUnits(units);

        if (granularity > 0)
            toEdit->setUpdateGranularity(granularity);

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
            QMessageBox errorMsg;
            errorMsg.setText("PV deleted unsuccessfully--unknown cause.");
            errorMsg.exec();
            return false;
        }

    } else {
        QMessageBox errorMsg;
        errorMsg.setText("Cannot delete pv at an invalid index.");
        errorMsg.exec();
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



void StripToolModel::toUpdateTime(int newTime)
{
    emit updateTime(newTime);
}



void StripToolModel::toUpdateTimeUnits(const QString &newUnits)
{
//    qDebug() << "The model received these units :" << newUnits;
    emit updateTimeUnits(newUnits); // update pvs.
    emit updateXAxisLabel("Time [" + newUnits + "]"); // update plot.
}


void StripToolModel::toSetSelectedWaterfall(double newWaterfall)
{
    if (selectedPV_) {
        qDebug() << "Setting waterfall value" << newWaterfall << "for pv" << selectedPV_->pvName() << ":" << setSelectedWaterfall(newWaterfall);
    }
}


bool StripToolModel::setSelectedWaterfall(double newWaterfall)
{
    if (newWaterfall == selectedPV_->waterfall()) {
        qDebug() << "New waterfall is same as old waterfall--no change made.";
        return false;
    }

    selectedPV_->setWaterfall(newWaterfall);
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



void StripToolModel::setSelectedPV(StripToolPV *newSelection)
{
    if (newSelection != selectedPV_)
    {
        if (newSelection && contains(newSelection))
        {
            // deselect old selection.
            if (selectedPV_ != 0)
                selectedPV_->setSelected(false);

            qDebug() << "Setting selected pv...";
            selectedPV_ = newSelection;
            selectedPV_->setSelected(true);

            emit modelSelectionChange();
            qDebug() << "Selected pv : " << selectedPV_->pvName();

        } else if (!newSelection) {

            if (selectedPV_ != 0)
                selectedPV_->setSelected(false);

            selectedPV_ = 0;
            emit modelSelectionChange();
            qDebug() << "Deselected pv.";

        } else {

            selectedPV_ = 0;
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
        qDebug() << "Editing metadata for pv" << pvName << "complete :" << toEdit->setMetaData(metaData);

    else
        qDebug() << "Matching pv not found.";
}



void StripToolModel::onSinglePVUpdated(QObject *pvUpdated)
{
    StripToolPV *updated = (StripToolPV *) pvUpdated;
    emit forceUpdatePVs(updated->pvName());
}



void StripToolModel::toTestSignal(const QString &signalText)
{
    qDebug() << "Signal text received by model :" << signalText;
}

