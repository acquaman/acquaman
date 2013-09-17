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

//    int updateNumber;
    QLabel *pvsAdded;
    QPair<QString, QString> newPVInfo;
    QList<QPair<QString, QString> > *activePVList_;
    QListView *activePVListView_;

    MPlot *plot;
    MPlotSeriesBasic *newSeries;
    MPlotRealtimeModel *newModel;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog *addPVDialog_;

protected:
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();
    int activePVCount();

protected slots:
    QList<QPair<QString, QString> > *getActivePVList();
    void onAddPVAction();
    void addToActivePVList(const QPair<QString, QString>);
    void onNewPVAccepted(const QPair<QString, QString>);
    void onNewPVCancelled();
    void onNewPVConnected(bool);
    void onNewPVUpdate(double);


};

#endif // STRIPTOOL_H
