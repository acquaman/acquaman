#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "beamline/AMPVControl.h"

class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    StripTool(QWidget *parent = 0);
    ~StripTool();

protected:
    QMenu *fileMenu_;
    QAction *quitAction_;

    QMenu *plotMenu_;
    QAction *addPVAction_;

    QMenu *viewMenu_;
    QAction *showPVListAction_;
    QAction *hidePVListAction_;

    QListWidget *pvList_;
    QDockWidget *pvDock_;

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
    void onShowPVListAction();
    void onHidePVListAction();
    void addToPVList(const QString &newPVName, const QString &newPVDescription);
    void onNewPVAccepted(const QString &newPVName, const QString &newPVDescription);
    void onNewPVCancelled();
    void onNewPVConnected(bool isConnected);
    void onNewPVUpdate(double newValue);

};

#endif // STRIPTOOL_H
