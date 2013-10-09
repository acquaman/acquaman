#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"
#include "StripChart/StripToolContainer.h"
#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolPVListModel.h"
#include "StripChart/StripToolSeries.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeriesData.h"


class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit StripTool(QWidget *parent = 0);
    ~StripTool();

signals:

protected:
    StripToolPVListModel *pvListModel_;
    StripToolContainer *itemContainer;
    StripToolPlot *plot_;
    AddPVDialog addPVDialog_;

    QListView *pvListView_;
    QDockWidget *pvDock_;

    QMenu *fileMenu_;
    QAction *newPlotAction_;
    QAction *saveDataAction_;
    QAction *savePVGroupAction_;
    QAction *quitAction_;

    QMenu *plotMenu_;
    QAction *addPVAction_;
    QAction *addSR1CurrentAction_;
    QAction *addPVGroupAction_;
    QAction *pausePVAction_;
    QAction *restartPVAction_;
    QAction *removePVAction_;
    QAction *deletePVAction_;
    QAction *showLessPVAction_;
    QAction *showMorePVAction_;

    QMenu *viewMenu_;
    QAction *toggleDockAction_;

protected:
    void createActions();
    void createMenus();
    void createPVDock();
    void addNewPV(const QString &newPVName, const QString &newPVDescription, const QString &newPVUnits);
    void deletePV(const QString &pvName, const QModelIndex &index);

protected slots:
    void onAddPVAction();
    void onAddSR1CurrentAction();
    void onAddPVGroupAction();
    void onPausePVAction();
    void onRestartPVAction();
    void onRemovePVAction();
    void onDeletePVAction();
    void onShowLessPVAction();
    void onShowMorePVAction();
    void onSavePVGroupAction();
    void onShowCheckedPV(const QString &pvName);
    void onRemoveUncheckedPV(const QString &pvName);
};

#endif // STRIPTOOL_H
