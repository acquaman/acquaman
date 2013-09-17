#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "StripChart/AddPVDialog.h"

#include "MPlot/MPlotWidget.h"
#include "beamline/AMPVControl.h"

class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    StripTool(QWidget *parent = 0);
    ~StripTool();

protected:
    QAction *quitAction_;
    QAction *addPVAction_;

    QMenu *fileMenu_;
    QMenu *plotMenu_;
    QMenu *viewMenu_;

    QList<QPair<QString, QString> > *activePVList_;

    QPushButton *addPVButton_;
    QPushButton *quitButton_;

    AddPVDialog *addPVDialog_;

protected:
    void createFileMenu();
    void createPlotMenu();
    void createViewMenu();

protected slots:
    QList<QPair<QString, QString> > *getActivePVList();
    void onAddPVAction();
    void addToActivePVList(const QPair<QString, QString>);
    void onNewPVAccepted(const QPair<QString, QString>);
    void onNewPVCancelled();


};

#endif // STRIPTOOL_H
