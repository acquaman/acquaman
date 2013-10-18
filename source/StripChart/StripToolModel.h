#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>

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
    void updateCheckState(Qt::CheckState);
    void updateUpdating(bool isUpdating);
    void updateValuesDisplayed(int valuesDisplayed);
    void updateDescription(const QString &newDescription);

protected:
    QList<StripToolPV*> pvList_;

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool contains(const QString &pvName);
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

protected slots:
    void addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void deletePV(const QModelIndex &index);
    void setValuesDisplayed(const QModelIndex &index, int valuesDisplayed);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    
};

#endif // STRIPTOOLMODEL_H
