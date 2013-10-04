#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"
#include "StripChart/StripToolItem.h"
#include "StripChart/StripToolContainer.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"

#include "beamline/AMPVControl.h"



class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    StripTool(QWidget *parent = 0);
    ~StripTool();

signals:

protected:
    QStandardItemModel *pvListModel_;
    QMap<QString, QString> pvNameToDescriptionMap_;
    QListView *pvListView_;

    StripToolContainer *itemContainer;

    QMenu *fileMenu_;
    QAction *newPlotAction_;
    QAction *openPlotAction_;
    QAction *saveDataAction_;
    QAction *savePlotAction_;
    QAction *setPVGroupAction_;
    QAction *quitAction_;

    QMenu *plotMenu_;
    QAction *addPVAction_;
    QAction *removePVAction_;
    QAction *addSR1CurrentAction_;
    QAction *addPVGroupAction_;

    QMenu *viewMenu_;
    QAction *togglePlotPaletteAction_;
    QAction *toggleLinePaletteAction_;
    QAction *togglePVListAction_;

    QMenu *settingsMenu_;

    MPlot *plot_;
    QDockWidget *pvDock_;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog addPVDialog_;

protected:
    void createPVListModel();
    void createPVDock();
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();
    void createSettingsMenu();
    void addPVToPlot(const QString &pvName);
    void removePVFromPlot(const QString &pvName);
    void deletePV(const QString &pvName);
    void updatePlotAxes(MPlotItem* plotSelection);

protected slots:
    void onAddPVAction();
    void onAddSR1CurrentAction();
    void onAddPVGroupAction();
    void onSetPVGroupAction();
    void addToPVListModel(const QString &newPVName, const QString &newPVDescription, const QString &newPVUnits);
    void togglePVVisibility(QStandardItem *entryChanged);
    void showItemInfo(MPlotItem* plotSelection);
};

#endif // STRIPTOOL_H
