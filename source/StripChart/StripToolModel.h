#ifndef STRIPTOOLMODEL_H
#define STRIPTOOLMODEL_H

#include <QAbstractListModel>
#include <QDebug>

#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolPV.h"
#include "StripChart/StripToolVariableInfo.h"
#include "StripChart/StripToolVariable.h"


class StripTool0DVariableInfo;

/// This class keeps track of all of the application's stored information. Each time a pv is created, the model creates an instance of StripToolPV and adds it to a QList. It carries out any changes the user makes to a particular pv and notifies views of the changes: plot and listview. Through the StripToolDataController, it also writes pv information to file so that pvs can be optionally reloaded when the application starts up again.

class StripToolModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StripToolModel(QObject *parent = 0);
    virtual ~StripToolModel();

signals:
//    void savePVData(QObject *toSave);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void modelSelectionChange();
//    void pvUpdating(const QModelIndex &index, bool isUpdating);
//    void updateTime(int newTime);
//    void updateTimeUnits(const QString &newUnits);
//    void updateXAxisLabel(const QString &newUnits);
    void selectedVariableDataRangeChanged(MPlotAxisRange *newRange);
    void selectedVariableDisplayRangeChanged(const MPlotAxisRange *newRange);
    void selectedVariableInfoChanged();
    void variableCheckStateChanged(const QModelIndex &index);

public:
    bool contains(const QString &nameToFind) const;
    StripToolVariable *findVariable(const QString &name) const;

    bool contains(const QModelIndex &indexToFind) const;
    StripToolVariable *findVariable(const QModelIndex &index) const;

    bool contains(MPlotItem *itemToFind) const;
    StripToolVariable* findVariable(MPlotItem *plotItem) const;
    StripToolVariable* findVariable(int row) const;

    bool contains(StripToolVariable *toMatch) const;
    bool contains(StripToolVariableInfo *toMatch) const;

    StripToolVariable* selectedVariable() const;

    MPlotItem* plotItem(int row) const;

public slots:
    bool addVariable(StripToolVariable *variable);
    bool addVariable(StripToolVariableInfo *infoSource);

    bool removeVariable(StripToolVariable *variable);
    bool deleteVariable(StripToolVariable *variable);
    void setSelectedVariable(StripToolVariable *newSelection);

    void enableWaterfall(bool isEnabled);
    void changeDisplayedTimeAmount(int amount);
    void changeDisplayedTimeUnits(const QString &units);

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

    void deselectSelectedVariable();
    void disconnectSelectedVariable();
    void selectVariable(StripToolVariable *newSelection);
    void connectSelectedVariable();

protected slots:

    /// Apply new line color selection.
    void colorPV(const QModelIndex &index, const QColor &color);

    /// Handles propagating the change in check state from the view's checkbox to the StripToolPV checkstate, and lets both the plot and the list view know a change has happened.
    void onCheckStateChanged(const QModelIndex &index, Qt::CheckState checked);

    /// Handles propagating the change in a pv's description from the view's line entry to the StripToolPV description field, and lets the list view know a change has happened.
    void onDescriptionChanged(const QModelIndex &index, const QString &newDescription);

private slots:
    void toTestRangeSignal(MPlotAxisRange* newRange);


private:
    void makeConnections();
    void defaultSettings();

private:
    QList<StripToolVariable*> variables_;
    StripToolVariable *selectedVariable_;

};

#endif // STRIPTOOLMODEL_H
