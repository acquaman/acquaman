#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "beamline/AMPVControl.h"
#include "MPlot/MPlotTools.h"


class StripToolView : public QMainWindow
{
    Q_OBJECT
    
public:
    StripToolView(QWidget *parent = 0);
    ~StripToolView();

signals:
    void hidePV(const QListWidgetItem &itemClicked);
    void showPV(const QListWidgetItem &itemClicked);

protected:
    QMenu *fileMenu_;
    QAction *newPlotAction_;
    QAction *openPlotAction_;
    QAction *saveDataAction_;
    QAction *savePlotAction_;
    QAction *quitAction_;

    QMenu *plotMenu_;
    QAction *addPVAction_;

    QMenu *viewMenu_;
    QAction *togglePlotPaletteAction_;
    QAction *toggleLinePaletteAction_;
    QAction *togglePVListAction_;

    int updateNumber_;
    int maxPointsDisplayed_;
    QVector<double> updateNumbers_;
    QVector<double> pvValues_;

    QListWidget *pvList_;
    QDockWidget *pvDock_;

    MPlotVectorSeriesData *pvSeriesData_;
    MPlotSeriesBasic *pvSeries_;
    MPlot *plot;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog *addPVDialog_;

protected:
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();
    void createPVDock();

protected slots:
    void onAddPVAction();
    void addToPVList(const QString &newPVName, const QString &newPVDescription);
    void onNewPVAccepted(const QString &newPVName, const QString &newPVDescription);
    void onNewPVCancelled();
    void onNewPVConnected(bool isConnected);
    void onNewPVUpdate(double newValue);
    void toTogglePVVisibility(const QListWidgetItem &itemClicked);
    void toEditPVDescription(const QListWidgetItem &itemDoubleClicked);
};

#endif // STRIPTOOL_H
