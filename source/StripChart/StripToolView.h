#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>
#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolQuickControls.h"

class StripToolView : public QWidget
{
    Q_OBJECT
    
public:
    explicit StripToolView(QWidget *parent = 0, StripToolModel *model = 0);
    ~StripToolView();

signals:
    void reloadPVs(bool reload);

protected:
    StripToolModel *model_;
    StripToolPlot *plotView_;
    StripToolQuickControls *quickControls_;

    QDir saveDirectory_;
    QString previousPVs_;

    QAction *newPlotAction_;
    QAction *saveDataAction_;
    QAction *quitAction_;

protected:
    void createActions();
    void buildUI();
    void reloadCheck();

protected slots:
    void toggleControls(int checkState);
    void toggleLegend(int checkState);

};

#endif // STRIPTOOLVIEW_H
