#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>

#include "StripChart/AddNewPVsDialog.h"
#include "StripChart/StripToolPlot.h"
//#include "StripChart/EditPVDialog.h"
#include "StripChart/StripToolModel.h"

#include "MPlot/MPlotWidget.h"


class StripToolView : public QWidget
{
    Q_OBJECT
    
public:
    explicit StripToolView(QWidget *parent = 0, StripToolModel *model = 0);
    ~StripToolView();

signals:
    void addNewPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void deletePV(const QModelIndex &index);
    void setPVUpdating(const QModelIndex &index, bool isUpdating);
    void setValuesDisplayed(const QModelIndex &index, int);

protected:
    StripToolModel *model_;
    StripToolPlot *plot_;
    AddNewPVsDialog addPVDialog_;
//    EditPVDialog editPVDialog_;
    QListView *pvListView_;

    QAction *newPlotAction_;
    QAction *saveDataAction_;
    QAction *savePVGroupAction_;
    QAction *quitAction_;
    QAction *addPVAction_;
    QAction *addSR1CurrentAction_;
    QAction *addPVGroupAction_;
    QAction *pausePVAction_;
    QAction *restartPVAction_;
    QAction *deletePVAction_;
    QAction *showLessPVAction_;
    QAction *showMorePVAction_;
    QAction *editPVAction_;
    QAction *toggleLegendAction_;

protected:
    void createActions();
    void buildUI();

protected slots:
    void onAddPVAction();
    void onAddSR1CurrentAction();
    void onPausePVAction();
    void onRestartPVAction();
    void onDeletePVAction();
//    void onShowLessPVAction();
//    void onShowMorePVAction();
//    void onEditPVAction();
};

#endif // STRIPTOOLVIEW_H
