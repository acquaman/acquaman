#include "StripToolPVListModel.h"

StripToolPVListModel::StripToolPVListModel(QObject *parent) : QObject (parent)
{
    pvList_ = new QStandardItemModel(this);
    connect(pvList_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onModelItemChanged(QStandardItem*)) );
}



StripToolPVListModel::~StripToolPVListModel()
{
}



QStandardItemModel* StripToolPVListModel::model()
{
    return pvList_;
}



void StripToolPVListModel::addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{

    QStandardItem *newEntry = new QStandardItem();
    newEntry->setChild(0, new QStandardItem(pvName));
    newEntry->setChild(1, new QStandardItem(pvUnits));
    newEntry->setCheckable(true);
    newEntry->setCheckState(Qt::Checked);
    newEntry->setToolTip(pvName);
    newEntry->setEditable(true);

    if (pvDescription == "")
        newEntry->setText(pvName);
    else
        newEntry->setText(pvDescription);

    pvList_->appendRow(newEntry);
}



void StripToolPVListModel::deletePV(const QModelIndex &index)
{
    pvList_->removeRow(index.row());
}



void StripToolPVListModel::toChangePVToChecked(const QString &pvName, const QModelIndex &index)
{
    Q_UNUSED(pvName);

    QStandardItem *toChange = pvList_->itemFromIndex(index);
    toChange->setCheckState(Qt::Checked);
}



QString StripToolPVListModel::pvNameFromIndex(const QModelIndex &index)
{
    return pvList_->itemFromIndex(index)->child(0)->text();
}



void StripToolPVListModel::onModelItemChanged(QStandardItem *changedItem)
{
    int checkState = changedItem->checkState();
    QString pvName = changedItem->child(0)->text();

    if (checkState == Qt::Checked)
        emit showCheckedPV(pvName);
    else if (checkState == Qt::Unchecked)
        emit removeUncheckedPV(pvName);
}


