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
    void seriesChanged(Qt::CheckState, MPlotItem*);
    void modelSelectionChange();
    void setPlotAxesLabels(const QString &xAxis, const QString &yAxis);
    void setPlotAxesRanges(const MPlotAxisRange &axisBottom);
    void setPlotTicksVisible(bool isShown);
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
    /// Returns the item flags for the entry at the given index.
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /// Returns the data that should be displayed when the given index is selected in a view--either the pv name or description.
    QVariant data(const QModelIndex &index, int role) const;
    /// Returns the header data, in case we switch to a view that uses header data.
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /// Returns the number of pvs in the model.
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /// Allows the user to set the pv description directly in the list view.
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /// Allows the user to edit the header data.
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    /// Returns false--no entries in the model have children.
    bool hasChildren(const QModelIndex &parent) const;

    /// Returns true if a pv item with a matching name is found, false otherwise.
    bool contains(const QString &nameToFind);
    /// Handles propagating the change in check state from the view's checkbox to the pv's saved checkstate, and lets both the plot and the list view know a change has happened.
    void checkStateChanged(const QModelIndex &index, Qt::CheckState checked);
    /// Handles propagating the change in a pv's description from the view's line entry to the pv's saved description field, and let's the list view know a change has happened.
    void descriptionChanged(const QModelIndex &index, const QString &newDescription);

protected slots:
    /// Creates a new StripToolPV object and assigns it the characteristics specified in pvName, pvDescription, and pvUnits.
    void addPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    /// Displays a dialog box that allows the user to edit a given pv(s) description and units.
    void editPV(QList<QModelIndex> indicesToEdit);
    /// Removes a pv from the model, list view, and plot.
    void deletePV(const QModelIndex &index);
    /// Selects the appropriate pv and causes it to pause/resume updating on the plot.
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    /// Selects the appropriate pv and changes the number of values it displays on the plot.
    void setValuesDisplayed(const QModelIndex &index, int points);
    /// Selects the appropriate pv and increments the number of values displayed.
    void incrementValuesDisplayed(const QModelIndex &index, int difference);
    /// An attempt to implement automatic saving of pv data--not fully implemented.
    void savePV(const QModelIndex &index);

    /// A series has been selected on the plot, the pv this series belongs to becomes the 'selectedPV_'.
    void seriesSelected(MPlotItem *plotSelection, bool isSelected);
    /// An item has been selected in the list view, the pv this item belongs to becomes the 'selectedPV_'.
    void itemSelected(const QModelIndex &index);
    /// Notifies the plot and the list view that one or the other has changed the model's selectedPV_.
    void onModelSelectionChange();
    /// Writes the pv name, description, and units of all pvs in the model to file (to be optionally reloaded later).
    void saveActivePVs();
    /// Reads the pv names, descriptons, and units from file and causes them to be reloaded back into the model.
    void reloadPVs(bool reload);
    /// An attempt to implement super basic color selection for pv.
    void colorPV(const QModelIndex &index, const QColor &color);
};

#endif // STRIPTOOLMODEL_H
