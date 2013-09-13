#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QtGui>

#include "addpvdialog.h"

#include "/Users/helfrij/dev/MPlot/src/MPlot/MPlotWidget.h"
#include "/Users/helfrij/dev/acquaman/source/beamline/AMPVControl.h"

class StripTool : public QMainWindow
{
    Q_OBJECT
    
public:
    StripTool(QWidget *parent = 0);
    ~StripTool();

protected:
    QAction *quitAction_;
    QAction *addPVAction_;

    QMenu *fileMenu;
    QMenu *plotMenu;
    QMenu *viewMenu;

    QList<QString> *activePVList;

    QPushButton *addPVButton;
    QPushButton *quitButton;

protected:

protected slots:
    void createActions();
    void createMenus();
    //void onAddPVAction();
    QList<QString> *getActivePVList();
    void addToActivePVList(const QString);

};

#endif // STRIPTOOL_H
