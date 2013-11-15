#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolPV.h"

/// This class keeps track of all of the application's stored information. Each time a pv is created, the model creates an instance of StripToolPV and adds it to a QList. It carries out any changes the user makes to a particular pv and notifies views of the changes: plot and listview. It also writes pv information to file so that pvs can be optionally reloaded when the application starts up again.

class StripToolModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StripToolModel(QObject *parent = 0);
    ~StripToolModel();

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void seriesChanged(Qt::CheckState state, int row);
    void modelSelectionChange();
    void setPlotAxesLabels(const QString &xAxis, const QString &yAxis);
    void setPlotAxesRanges(const MPlotAxisRange &axisBottom);
    void setPlotTicksVisible(bool isShown);
    void showReloadDialog();
    void errorMessage(const QString &message);
    void pvValid(bool isValid);

protected:
    QList<StripToolPV*> pvList_;
    StripToolPV *selectedPV_;

    QDir appDirectory_;

    QSignalMapper *controlMapper_;
    QSignalMapper *pvMapper_;

public:
    StripToolPV* selectedPV() const;
    QModelIndex selectedIndex() const;
    MPlotItem* selectedSeries() const;
    MPlotItem* series(int row) const;

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

    /// Returns true if a pv item with a matching name is found, false otherwise.
    bool contains(const QString &nameToFind);
    bool contains(StripToolPV *toMatch) const;
    /// Handles propagating the change in check state from the view's checkbox to the StripToolPV checkstate, and lets both the plot and the list view know a change has happened.
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);
    /// Handles propagating the change in a pv's description from the view's line entry to the StripToolPV description field, and lets the list view know a change has happened.
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

    /// Removes a pv from the model, list view, and plot.
    bool deletePV(const QModelIndex &index);
    /// Creates a new StripToolPV object and sets its control to pvControl.
    bool addPV(AMControl *pvControl);
    bool savePVData(StripToolPV *toSave);

    StripToolPV* findItem(MPlotItem* series);
    StripToolPV* findItem(const QString &pvName);

    void setSelectedPV(StripToolPV *newSelection);

protected slots:
    void toAddPV(const QString &pvName);
    void onPVConnected(QObject *itemConnected);
    void toDeletePV(const QModelIndex &index);
    void toSavePVData(QObject *toSave);

    /// Displays a dialog box that allows the user to edit a given pv(s) description and units.
    void editPV(QList<QModelIndex> indicesToEdit);

    /// Selects the appropriate pv and causes it to pause/resume updating on the plot.
    void setPVUpdating(const QModelIndex &index, bool isUpdating);

    /// Selects the appropriate pv and changes the number of values it displays on the plot.
    void setValuesDisplayed(const QModelIndex &index, int points);

    /// Writes the pv name, description, and units of all pvs in the model to file (to be optionally reloaded later).
    void toUpdatePVFile();

    /// Reads the pv names, descriptons, and units from file and causes them to be reloaded back into the model.
    void reloadPVs(bool reload);

    /// Basic color selection for pv.
    void colorPV(const QModelIndex &index, const QColor &color);

    void seriesSelected(MPlotItem*);
    void seriesDeselected();

    void listItemSelected(const QModelIndex &newSelection, const QModelIndex &oldSelection);
    void listItemDeselected();

    void onModelSelectionChange();

    void reloadCheck();


};

#endif // STRIPTOOLMODEL_H
