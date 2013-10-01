#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"
#include "StripChart/StripToolItem.h"

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

    QListView *pvListView_;
    QTableView *pvTableView_;
    QDockWidget *pvDock_;

    QStandardItemModel *pvDataModel_;

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

    int updateNumber_;
    int maxPointsDisplayed_;
    QVector<double> updateNumbers_;
    QVector<double> pvValues_;

    MPlotVectorSeriesData *pvSeriesData_;
    MPlotSeriesBasic *pvSeries_;
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

protected slots:
    void onAddPVAction();
    void addToPVListModel(const QString &newPVName, const QString &newPVDescription);
    void onPVValueChanged(double);
    void toTogglePVVisibility(QStandardItem *changedItem);
};

#endif // STRIPTOOL_H
