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

    QLabel *pvsAdded;
//    QList<QString> *activePVList_;
//    QListView *activePVListView_;
    QListWidget *activePVList_;

    MPlot *plot;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog *addPVDialog_;

protected:
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();

protected slots:
    void onAddPVAction();
    void addToActivePVList(QString, QString);
    void onNewPVAccepted(QString, QString);
    void onNewPVCancelled();
    void onNewPVConnected(bool);
    void onNewPVUpdate(double);

};

#endif // STRIPTOOL_H
