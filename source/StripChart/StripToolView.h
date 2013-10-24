#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>

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
    void addNewPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void editPV();

protected:
    StripToolModel *model_;
    StripToolPlot *plotView_;
    StripToolQuickControls *quickControls_;

    QAction *newPlotAction_;
    QAction *saveDataAction_;
    QAction *quitAction_;

protected:
    void createActions();
    void buildUI();

protected slots:
    void toggleControls(int checkState);
    void toggleLegend(int checkState);

};

#endif // STRIPTOOLVIEW_H
