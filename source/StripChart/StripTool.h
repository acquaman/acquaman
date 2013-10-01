#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"
#include "StripChart/StripToolItem.h"
#include "StripChart/StripToolContainer.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "beamline/AMPVControl.h"
#include "MPlot/MPlotTools.h"


class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    StripTool(QWidget *parent = 0);
    ~StripTool();

signals:
    void hidePV(const QListWidgetItem &itemClicked);
    void showPV(const QListWidgetItem &itemClicked);

protected:
    QStandardItemModel *pvListModel_;
    QList<StripToolItem *> itemList_;
    StripToolContainer *itemContainer;
    QMap<QString, MPlotVectorSeriesData *> pvNameToDataMap;
    QMap<QString, MPlotSeriesBasic *> pvNameToSeriesMap;

    QListView *pvListView_;
    QDockWidget *pvDock_;

    QMenu *fileMenu_;
    QAction *newPlotAction_;
    QAction *openPlotAction_;
    QAction *saveDataAction_;
    QAction *savePlotAction_;
    QAction *quitAction_;

    QMenu *plotMenu_;
    QAction *addPVAction_;
    QAction *removePVAction_;

    QMenu *viewMenu_;
    QAction *togglePlotPaletteAction_;
    QAction *toggleLinePaletteAction_;
    QAction *togglePVListAction_;

    int maxPointsDisplayed_;

    MPlot *plot;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog *addPVDialog_;

protected:
    void createPVListModel();
    void createPVDock();
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();
    void addNewPVToPlot();

protected slots:
    void onAddPVAction();
    void addToPVListModel(const QString &newPVName, const QString &newPVDescription);
    void updatePVData(const QString &pvName, QVector<double> xValues, QVector<double> yValues);
};

#endif // STRIPTOOL_H
