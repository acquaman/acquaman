#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolPV.h"


/// This class keeps track of all of the application's stored information. Each time a pv is created, the model creates an instance of StripToolPV and adds it to a QList. It carries out any changes the user makes to a particular pv and notifies views of the changes: plot and listview. Through the StripToolDataController, it also writes pv information to file so that pvs can be optionally reloaded when the application starts up again.

class StripToolModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StripToolModel(QObject *parent = 0);
    ~StripToolModel();

signals:
    void metaDataCheck(const QString &pvName);
    void savePVData(QObject *toSave);
    void savePVMetadata(QObject *toSave);
    void deletePVData(QObject *toDelete);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void seriesChanged(Qt::CheckState state, int row);
    void modelSelectionChange();
    void pvUpdating(const QModelIndex &index, bool isUpdating);
    void updateTime(int newTime);
    void updateTimeUnits(const QString &newUnits);
    void updateXAxisLabel(const QString &newUnits);
    void requestTimeUpdate();
    void selectedDataMaxChanged(double newMax);
    void selectedDataMinChanged(double newMin);
    void selectedCustomDataMaxChanged(double newMax);
    void selectedCustomDataMinChanged(double newMin);
    void selectedPVDataRangeChanged(MPlotAxisRange *newRange);
    void selectedPVDisplayRangeChanged(MPlotAxisRange *newRange);
    void selectedPVAxisLabelChanged(const QString &newLabel);
    void changeWaterfallCheckState(bool on);
    void waterfallStateChanged(bool on);


protected:
    QList<StripToolPV*> pvList_;
    StripToolPV *selectedPV_;
    QSignalMapper *saveDataMapper_;
    QSignalMapper *saveMetadataMapper_;
    QSignalMapper *pvUpdatedMapper_;

public:
    StripToolPV* selectedPV() const;
    QModelIndex selectedIndex() const;
    MPlotItem* selectedSeries() const;
    QString selectedDescription() const;
    QString selectedUnits() const;
    MPlotItem* series(int row) const;
    QString pvName(StripToolPV *pv) const;

    AMProcessVariable* pv_;
    AM0DProcessVariableDataSource* dataSource_;


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

    StripToolPV* findItem(MPlotItem* series) const;
    StripToolPV* findItem(const QString &pvName) const;

    /// Returns true if a pv item with a matching name is found, false otherwise.
    bool contains(const QString &nameToFind) const;

    bool contains(StripToolPV *toMatch) const;

    /// Handles propagating the change in check state from the view's checkbox to the StripToolPV checkstate, and lets both the plot and the list view know a change has happened.
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);

    /// Handles propagating the change in a pv's description from the view's line entry to the StripToolPV description field, and lets the list view know a change has happened.
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

    /// Removes a pv from the model, list view, and plot.
    bool deletePV(const QModelIndex &index);

    /// Creates a new StripToolPV object and sets its control to pvControl.
    bool addPV(AMDataSource *dataSource);
    void setSelectedPV(StripToolPV *newSelection);

    void deselectPV();
    void selectPV(StripToolPV *newSelection);

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

    void seriesSelected(MPlotItem*);
    void seriesDeselected();

    void listItemSelected(const QModelIndex &newSelection, const QModelIndex &oldSelection);

    void toSetMetaData(const QString &pvName, QList<QString> metaData);

    void toTestSignal(const QString &signalText);
    void toTestDoubleSignal(double val);
    void toTestRangeSignal(MPlotAxisRange* newRange);

    ////////

    void toChangeYAxisLabel();

    void onPVValueChanged(double newValue);
    void onDataSourceValueChanged(AMnDIndex start, AMnDIndex end);

};

#endif // STRIPTOOLMODEL_H
