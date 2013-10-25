#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>

#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolPV.h"

class StripToolModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StripToolModel(QObject *parent = 0);
    ~StripToolModel();
    
signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void seriesChanged(Qt::CheckState, MPlotItem*);
    void modelSelectionChange();
    void setPlotAxesLabels(const QString &xAxis, const QString &yAxis);
    void setSeriesSelected(MPlotItem *series);
    void setItemSelected(const QModelIndex &index);

protected:
    QList<StripToolPV*> pvList_;
    QList<QModelIndex> itemList_;
    StripToolPV *selectedPV_;
    QModelIndex selectedIndex_;

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool contains(const QString &nameToFind);
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

protected slots:
    void addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void editPV(QList<QModelIndex> indicesToEdit);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void setValuesDisplayed(const QModelIndex &index, int points);
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    void seriesSelected(MPlotItem *plotSelection, bool isSelected);
    void itemSelected(const QModelIndex &index);
    void onModelSelectionChange();
};

#endif // STRIPTOOLMODEL_H
