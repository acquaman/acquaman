#include "StripToolModel.h"

#include "StripChart/StripToolBasicVariableInfo.h"

StripToolModel::StripToolModel(QObject *parent) : QAbstractListModel(parent)
{
    selectedPV_ = 0;

    // used when the StripToolPV signals that it is ready to save its collected data.
    saveDataMapper_ = new QSignalMapper(this);
//    connect( saveDataMapper_, SIGNAL(mapped(QObject*)), this, SIGNAL(savePVData(QObject*)) );

    // used when the StripToolPV signals that it is ready for changes to its metadata to be saved.
    saveMetadataMapper_ = new QSignalMapper(this);
//    connect( saveMetadataMapper_, SIGNAL(mapped(QObject*)), this, SIGNAL(savePVMetadata(QObject*)) );

//    connect( this, SIGNAL(pvUpdating(QModelIndex,bool)), this, SLOT(setPVUpdating(QModelIndex,bool)) );

    connect( this, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(toTestRowsInserted(QModelIndex, int, int)) );
}



StripToolModel::~StripToolModel()
{
}



StripToolBasicVariable *StripToolModel::selectedVariable() const
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



MPlotItem* StripToolModel::selectedPlotItem() const
{
    if (selectedPV_ == 0)
        return 0;
    else
        return selectedPV_->plotItem();
//    return 0;
}


QString StripToolModel::selectedDescription() const
{
    if (selectedPV_ == 0)
        return "";
    else
        return selectedPV_->description();
}


QString StripToolModel::selectedUnits() const
{
    if (selectedPV_ == 0)
        return "";
    else
        return selectedPV_->units();
}



MPlotItem* StripToolModel::plotItem(int row) const
{
    MPlotItem *series = 0;

    if (row >= 0 && row < pvList_.size())
        series = pvList_.at(row)->plotItem();

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

    StripToolBasicVariable *toDisplay = pvList_.at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (toDisplay->description() == "")
            return QVariant(toDisplay->name());
        else
            return QVariant(toDisplay->description());
    }

    if (role == Qt::ToolTipRole)
        return QVariant(toDisplay->name());

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



StripToolBasicVariable* StripToolModel::findVariable(const QString &pvName) const
{
    StripToolBasicVariable *match = 0;
    bool found = false;
    int count = 0;

    while(count < pvList_.size() && !found) {
        if (pvName == pvList_.at(count)->name()) {
            match = pvList_.at(count);
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


StripToolBasicVariable* StripToolModel::findVariable(const QModelIndex &index) const {
    if (index == QModelIndex())
        return 0;

    if (index.row() >= pvList_.size())
        return 0;

    if (index.row() < 0)
        return 0;

    else
        return pvList_.at(index.row());
}



bool StripToolModel::contains(MPlotItem *itemToFind) const {
    if (findVariable(itemToFind) == 0)
        return false;
    else
        return true;
}



StripToolBasicVariable* StripToolModel::findVariable(MPlotItem *plotItem) const
{
    StripToolBasicVariable *match = 0;
    bool found = false;
    int count = 0;

    while (count < pvList_.size() && !found) {
        if (plotItem == pvList_.at(count)->plotItem()) {
            match = pvList_.at(count);
            found = true;
        }

        count++;
    }

    return match;
}



bool StripToolModel::contains(StripToolBasicVariable *toMatch) const
{
    bool matchFound = false;
    int count = 0;

    while (count < pvList_.size() && !matchFound) {
        if (toMatch == pvList_.at(count))
            matchFound = true;

        count++;
    }

    return matchFound;
}



void StripToolModel::onCheckStateChanged(const QModelIndex &index, Qt::CheckState checked)
{
    int row = index.row();

    if (index.isValid() && row < pvList_.size()) {
        StripToolBasicVariable *toChange = qobject_cast<StripToolBasicVariable*>(pvList_.at(row));
        toChange->setCheckState(checked);

        emit variableCheckStateChanged(checked, row);
        emit dataChanged(index, index);
    }
}



void StripToolModel::onDescriptionChanged(const QModelIndex &index, const QString &newDescription)
{
    StripToolPV *toChange = qobject_cast<StripToolPV*>(pvList_.at(index.row()));
    toChange->setDescription(newDescription);
    emit dataChanged(index, index);
}



void StripToolModel::toAddPV(const QString &sourceName)
{
    qDebug() << "Pv to add : " << sourceName;

    if (sourceName == "") {
        QMessageBox errorMsg;
        errorMsg.setText("PV name cannot be blank.");
        errorMsg.exec();

    } else if (contains(sourceName)) {
        QMessageBox errorMsg;
        errorMsg.setText("Cannot add duplicate pvs.");
        errorMsg.setDetailedText("PV with that name already added!");
        errorMsg.exec();

    } else {

        AM0DProcessVariableDataSource *dataSource = new AM0DProcessVariableDataSource(new AMProcessVariable(sourceName, true), sourceName, this);
        qDebug() << "Adding new pv..." << addPV(dataSource);

    }
}



bool StripToolModel::addPV(AMDataSource *dataSource)
{
    int position = pvList_.size(); // append new pvs to the end of the model.
    int count = 1; // insert one pv at a time.

    qDebug() << "About to create StripTool0DVariable object...";
    StripTool0DVariable *newPV = new StripTool0DVariable(new StripTool0DVariableInfo(), dataSource, this);

//    qDebug() << "Setting up signal mappers for new pv.";

//    saveDataMapper_->setMapping(newPV, newPV);
//    saveMetadataMapper_->setMapping(newPV, newPV);
//    pvUpdatedMapper_->setMapping(newPV, newPV);

//    qDebug() << "About to set connections for new pv.";

//    connect( newPV, SIGNAL(savePVData()), saveDataMapper_, SLOT(map()) );
//    connect( newPV, SIGNAL(savePVMetaData()), saveMetadataMapper_, SLOT(map()) );

//    connect( this, SIGNAL(updateTime(int)), newPV, SLOT(toUpdateTime(int)) );
//    connect( this, SIGNAL(updateTimeUnits(QString)), newPV, SLOT(toUpdateTimeUnits(QString)) );

    qDebug() << "About to insert StripToolPV into the model...";
    beginInsertRows(QModelIndex(), position, position + count - 1); //  notify list view and plot.
    pvList_.insert(position, newPV); // add new pv to the model.
    endInsertRows();

    if (position + count == pvList_.size()) {
        setSelectedPV(newPV);

//        qDebug() << "Requesting meta data for pv" << newPV->name() << "if it exists...";
//        emit metaDataCheck(newPV->name());

//        qDebug() << "Requesting time update.";
//        emit requestTimeUpdate();

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

//    StripToolBasicVariable *toEdit = pvList_.at(index.row());

//    EditPVDialog *editDialog = new EditPVDialog(toEdit->info());
//    connect( this, SIGNAL(selectedPVDataRangeChanged(MPlotAxisRange*)), editDialog, SLOT(toUpdateDataRange(MPlotAxisRange*)) );

//    if (editDialog->exec()) {

//        if (editDialog->descriptionChanged())
//        {
//            toEdit->setDescription(editDialog->description());
//            emit dataChanged(index, index);
//        }

//        if (editDialog->unitsChanged())
//            toEdit->setUnits(editDialog->units());

//        if (editDialog->granularityChanged())
//            toEdit->setUpdateGranularity(editDialog->granularity());

//        if (editDialog->colorChanged())
//            toEdit->setColorName(editDialog->color());

//        if (editDialog->displayMaxChanged()) {
//            QString displayMax = editDialog->displayMax();
//            toEdit->setDisplayedYMax(displayMax);
//        }

//        if (editDialog->displayMinChanged()) {
//            QString displayMin = editDialog->displayMin();
//            toEdit->setDisplayedYMin(displayMin);
//        }

//    }

//    editDialog->deleteLater();
}



void StripToolModel::toDeletePV(const QModelIndex &index)
{
    qDebug() << "Deleting pv : " << deletePV(index);
}



bool StripToolModel::deletePV(const QModelIndex &index)
{
//    int size = pvList_.size();

//    if (index.isValid() && index.row() < size)
//    {
//        int position = index.row(); //  use the index to identify which pv to remove from the list.
//        int count = 1; //  delete pvs one at a time.

//        setSelectedPV(0);
////        emit deletePVData(pvList_.at(index.row()));
//        pvList_.at(position)->disconnect(this);

//        beginRemoveRows(QModelIndex(), position, position + count - 1); //  notify model view.
//        pvList_.takeAt(position)->deleteLater(); //  remove pv from model.
//        endRemoveRows();

//        if (size == pvList_.size() + 1)
//        {
//            return true;

//        } else {
//            QMessageBox errorMsg;
//            errorMsg.setText("PV deleted unsuccessfully--unknown cause.");
//            errorMsg.exec();
//            return false;
//        }

//    } else {
//        QMessageBox errorMsg;
//        errorMsg.setText("Cannot delete pv at an invalid index.");
//        errorMsg.exec();
//        return false;
//    }


}



void StripToolModel::toPausePVs()
{
//    int pvCount = pvList_.size();

//    for (int i = 0; i < pvCount; i++)
//    {
//        emit pvUpdating(createIndex(i, 0), false);
//    }
}



void StripToolModel::toResumePVs()
{
//    int pvCount = pvList_.size();

//    for (int i = 0; i < pvCount; i++)
//    {
//        emit pvUpdating(createIndex(i, 0), true);
//    }
}



void StripToolModel::toUpdateTime(int newTime)
{
//    emit updateTime(newTime);
}



void StripToolModel::toUpdateTimeUnits(const QString &newUnits)
{
////    qDebug() << "The model received these units :" << newUnits;
//    emit updateTimeUnits(newUnits); // update pvs.
//    emit updateXAxisLabel("Time [" + newUnits + "]"); // update plot.
}



void StripToolModel::setPVUpdating(const QModelIndex &index, bool isUpdating)
{
    if (index.isValid() && index.row() < pvList_.size())
    {
        StripToolPV *toChange = qobject_cast<StripToolPV*>(pvList_.at(index.row()));
        toChange->setPVUpdating(isUpdating);
    }
}



void StripToolModel::setSelectedVariable(const QModelIndex &newSelection) {
    if (newSelection == QModelIndex())
        deselectPV();

    else if (newSelection.row() >= pvList_.size())
        deselectPV();

    else
        setSelectedPV(pvList_.at(newSelection.row()));
}



void StripToolModel::setSelectedVariable(MPlotItem *newSelection) {
    if (newSelection == 0)
        deselectPV();

    else if (!contains(newSelection))
        deselectPV();

    else
        setSelectedPV(findVariable(newSelection));
}



void StripToolModel::setSelectedPV(StripToolBasicVariable *newSelection)
{
    if (newSelection != selectedPV_) {

        if (newSelection && contains(newSelection)) {
            deselectPV();
            selectPV(newSelection);

        } else if (!newSelection) {
            if (selectedPV_ != 0)
                deselectPV();

        } else {
            selectedPV_ = 0;
            qDebug() << "StripToolModel :: Attempting to set an unknown pv as selected!! No change made.";
        }
    }
}



void StripToolModel::deselectPV()
{
    if (selectedPV_ != 0) {

        qDebug() << "StripToolModel :: the current selection is not 0--deselecting currently selected variable.";

        selectedPV_->setSelectionState(false);

        disconnect( selectedPV_, SIGNAL(dataRangeUpdated(MPlotAxisRange*)), this, SIGNAL(selectedPVDataRangeChanged(MPlotAxisRange*)) );
        disconnect( selectedPV_, SIGNAL(displayRangeUpdated(MPlotAxisRange*)), this, SIGNAL(selectedPVDisplayRangeChanged(MPlotAxisRange*)) );
        disconnect( selectedPV_, SIGNAL(descriptionChanged(QString)), this, SLOT(toChangeYAxisLabel()) );
        disconnect( selectedPV_, SIGNAL(unitsChanged(QString)), this, SLOT(toChangeYAxisLabel()) );

        selectedPV_ = 0;

        qDebug() << "StripToolModel :: Deselected pv.";

        emit modelSelectionChange();
    }
}



void StripToolModel::selectPV(StripToolBasicVariable *newSelection)
{
    if (selectedPV_ != 0) {
        deselectPV();
    }

    qDebug() << "Setting selected pv...";

    selectedPV_ = newSelection;
    selectedPV_->setSelectionState(true);

    connect( selectedPV_, SIGNAL(dataRangeUpdated(MPlotAxisRange *)), this, SIGNAL(selectedPVDataRangeChanged(MPlotAxisRange *)) );
    connect( selectedPV_, SIGNAL(displayRangeUpdated(MPlotAxisRange*)), this, SIGNAL(selectedPVDisplayRangeChanged(MPlotAxisRange*)) );
    connect( selectedPV_, SIGNAL(descriptionChanged(QString)), this, SIGNAL(selectedVariableInfoChanged()) );
    connect( selectedPV_, SIGNAL(unitsChanged(QString)), this, SIGNAL(selectedVariableInfoChanged()) );

    qDebug() << "Selected pv : " << selectedPV_->name();

    emit modelSelectionChange();
}



void StripToolModel::toSetSelectedVariable(MPlotItem *plotSelection)
{
    setSelectedPV(findVariable(plotSelection));
}



//void StripToolModel::seriesDeselected()
//{
//    setSelectedPV(0);
//}



void StripToolModel::toSetSelectedVariable(const QModelIndex &newSelection, const QModelIndex &oldSelection)
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
        StripToolPV *toEdit = qobject_cast<StripToolPV*>(pvList_.at(index.row()));
        toEdit->setColor(color.name());
    }
}



//void StripToolModel::toSetMetaData(const QString &pvName, QList<QString> metaData)
//{
//    qDebug() << "Meta data found. Attempting to find matching pv...";
//    StripToolPV *toEdit = qobject_cast<StripToolPV*>(findItem(pvName));

//    if (toEdit != 0)
//        qDebug() << "Editing metadata for pv" << pvName << "complete :" << toEdit->setMetaData(metaData);

//    else
//        qDebug() << "Matching pv not found.";
//}



void StripToolModel::toTestSignal(const QString &signalText)
{
    qDebug() << "Signal text received by model :" << signalText;
}



void StripToolModel::toTestDoubleSignal(double val)
{
    qDebug() << "Signal double received by model :" << val;
}



void StripToolModel::toTestRangeSignal(MPlotAxisRange *newRange)
{
    qDebug() << "StripToolModel :: the model received a new signal range with min ->" << newRange->min() << "and max ->" << newRange->max();
}



void StripToolModel::toTestRowsInserted(const QModelIndex &index, int start, int end) {
    Q_UNUSED(index)
    Q_UNUSED(start)
    Q_UNUSED(end)

    qDebug() << "A row was just inserted into the model! Hooray!";
}

