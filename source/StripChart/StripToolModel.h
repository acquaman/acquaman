#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>
#include <QDebug>

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
    void updateActivePVs();

protected:
    QList<QVariant> headerData_;
    QList<StripToolPV*> pvList_;
    StripToolPV *selectedPV_;
    QModelIndex selectedIndex_;
    QDir saveDirectory_;
    QString pvFilename_;

public:
    void setSaveDirectory(QDir &newDir);
    void setPVFilename(const QString &pvFilename);

protected:
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    bool hasChildren(const QModelIndex &parent) const;

    bool contains(const QString &nameToFind);
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

protected slots:
    void addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void editPV(QList<QModelIndex> indicesToEdit);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void setValuesDisplayed(const QModelIndex &index, int points);
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    void savePV(const QModelIndex &index);

    void seriesSelected(MPlotItem *plotSelection, bool isSelected);
    void itemSelected(const QModelIndex &index);
    void onModelSelectionChange();
    void saveActivePVs();
    void reloadPVs(bool reload);
};

#endif // STRIPTOOLMODEL_H
