#ifndef STRIPTOOLPVLISTMODEL_H
#define STRIPTOOLPVLISTMODEL_H

#include <QStandardItemModel>

class StripToolPVListModel : public QObject
{
    Q_OBJECT

public:
    explicit StripToolPVListModel(QObject *parent = 0);
    ~StripToolPVListModel();
    friend class StripTool;

signals:
    void showCheckedPV(const QString &pvName);
    void removeUncheckedPV(const QString &pvName);

protected:
    QStandardItemModel *pvList_;

protected:
    QStandardItemModel* model();
    void addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void deletePV(const QModelIndex &index);
    QString pvNameFromIndex(const QModelIndex &index);

protected slots:
    void onModelItemChanged(QStandardItem *changedItem);
    void toChangePVToChecked(const QString &pvName, const QModelIndex &index);
};

#endif // STRIPTOOLPVLISTMODEL_H
