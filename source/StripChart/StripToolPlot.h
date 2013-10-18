#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QtGui>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotWidget.h"

class StripToolPlot : public MPlotWidget
{
    Q_OBJECT

public:
    explicit StripToolPlot(QWidget *parent = 0);
    ~StripToolPlot();
    
signals:

protected:
    MPlot *plot_;
    MPlotPlotSelectorTool *selector_;

protected:
    MPlot* plot();
    bool contains(MPlotItem *series);
    void addSeriesToPlot(MPlotItem *newSeries);
    void removeSeriesFromPlot(MPlotItem *toRemove);
    void showPlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel);
    void hidePlotAxesLabels();
    
protected slots:
    void onSeriesSelected(MPlotItem* plotSelection);
    void onSeriesDeselected();
    void onSeriesChanged(Qt::CheckState seriesState, MPlotItem *series);

};

#endif // STRIPTOOLPLOT_H
