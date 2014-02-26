#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolPV.h"
#include "StripChart/StripTool0DVariable.h"


class StripTool0DVariableInfo;

/// This class keeps track of all of the application's stored information. Each time a pv is created, the model creates an instance of StripToolPV and adds it to a QList. It carries out any changes the user makes to a particular pv and notifies views of the changes: plot and listview. Through the StripToolDataController, it also writes pv information to file so that pvs can be optionally reloaded when the application starts up again.

class StripToolModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StripToolModel(QObject *parent = 0);
    ~StripToolModel();
    friend class StripTool;

signals:
//    void metaDataCheck(const QString &pvName);
//    void savePVData(QObject *toSave);
//    void savePVMetadata(QObject *toSave);
//    void deletePVData(QObject *toDelete);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void modelSelectionChange();
//    void pvUpdating(const QModelIndex &index, bool isUpdating);
//    void updateTime(int newTime);
//    void updateTimeUnits(const QString &newUnits);
//    void updateXAxisLabel(const QString &newUnits);
//    void requestTimeUpdate();

//    void selectedDataMaxChanged(double newMax);
//    void selectedDataMinChanged(double newMin);
//    void selectedCustomDataMaxChanged(double newMax);
//    void selectedCustomDataMinChanged(double newMin);
    void selectedVariableDataRangeChanged(MPlotAxisRange *newRange);
    void selectedVariableDisplayRangeChanged(MPlotAxisRange *newRange);
    void selectedVariableInfoChanged();
    void variableCheckStateChanged(Qt::CheckState state, int row);


//    void selectedPVAxisLabelChanged(const QString &newLabel);
//    void changeWaterfallCheckState(bool on);
//    void waterfallStateChanged(bool on);

protected:
    bool contains(const QString &nameToFind) const;
    StripToolBasicVariable *findVariable(const QString &name) const;

    bool contains(const QModelIndex &indexToFind) const;
    StripToolBasicVariable *findVariable(const QModelIndex &index) const;

    bool contains(MPlotItem *itemToFind) const;
    StripToolBasicVariable* findVariable(MPlotItem *plotItem) const;

    bool contains(StripToolBasicVariable *toMatch) const;

    StripToolBasicVariable* selectedVariable() const;
    QModelIndex selectedIndex() const;
    MPlotItem* selectedPlotItem() const;
    QString selectedDescription() const;
    QString selectedUnits() const;

    MPlotItem* plotItem(int row) const;

protected:
    /// Returns the item flags for the entry at the given index.
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /// Returns the data that should be displayed when the given index is selected in a view--either the pv name or description.
    QVariant data(const QModelIndex &index, int role) const;

    /// Returns the number of pvs in the model.
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /// Allows the user to set the pv description directly in the list view.
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /// Returns false--no entries in the model have children.
    bool hasChildren(const QModelIndex &parent) const;

    /// Removes a pv from the model, list view, and plot.
    bool deletePV(const QModelIndex &index);

    /// Creates a new StripToolPV object and sets its control to pvControl.
    bool addPV(AMDataSource *dataSource);

    void setSelectedVariable(const QModelIndex &newSelection);
    void setSelectedVariable(MPlotItem* newSelection);

    void setSelectedPV(StripToolBasicVariable *newSelection);
    void deselectPV();
    void selectPV(StripToolBasicVariable *newSelection);

protected slots:
    void toAddPV(const QString &sourceName);
    void toDeletePV(const QModelIndex &index);
    void toPausePVs();
    void toResumePVs();
    void toUpdateTime(int newTime);
    void toUpdateTimeUnits(const QString &newUnits);

    /// Displays a dialog box that allows the user to edit a given pv(s) description and units.
    void editPV(const QModelIndex &indexToEdit);

    /// Selects the appropriate pv and causes it to pause/resume updating on the plot.
    void setPVUpdating(const QModelIndex &index, bool isUpdating);

    /// Basic color selection for pv.
    void colorPV(const QModelIndex &index, const QColor &color);

//    void seriesSelected(MPlotItem*);
//    void seriesDeselected();

    void toSetSelectedVariable(MPlotItem* plotSelection);
    void toSetSelectedVariable(const QModelIndex &newSelection, const QModelIndex &oldSelection);

//    void listItemSelected(const QModelIndex &newSelection, const QModelIndex &oldSelection);
    /// Handles propagating the change in check state from the view's checkbox to the StripToolPV checkstate, and lets both the plot and the list view know a change has happened.
    void onCheckStateChanged(const QModelIndex &index, Qt::CheckState checked);

    /// Handles propagating the change in a pv's description from the view's line entry to the StripToolPV description field, and lets the list view know a change has happened.
    void onDescriptionChanged(const QModelIndex &index, const QString &newDescription);

//    void toSetMetaData(const QString &pvName, QList<QString> metaData);

    ////////

//    void toChangeYAxisLabel();

private slots:
    void toTestRowsInserted(const QModelIndex &index, int start, int end);
    void toTestSignal(const QString &signalText);
    void toTestDoubleSignal(double val);
    void toTestRangeSignal(MPlotAxisRange* newRange);


private:
    QList<StripToolBasicVariable*> pvList_;
    StripToolBasicVariable *selectedPV_;
    QSignalMapper *saveDataMapper_;
    QSignalMapper *saveMetadataMapper_;
    QSignalMapper *pvUpdatedMapper_;

};

#endif // STRIPTOOLMODEL_H
