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
    void seriesSelected(MPlotItem *plotSelection, bool isSelected);

protected:
    MPlot *plot_;
    MPlotPlotSelectorTool *selector_;
    MPlotItem *plotSelection_;

protected:
    bool contains(MPlotItem *series);
    void addSeriesToPlot(MPlotItem *newSeries);
    void removeSeriesFromPlot(MPlotItem *toRemove);
    void hidePlotAxesLabels();
    
protected slots:
    void setSeriesSelection(MPlotItem *modelSelection);
    void onSeriesSelected(MPlotItem *plotSelection);
    void onSeriesDeselected();
    void setPlotAxesLabels(const QString &bottomLabel, const QString &leftLabel);
    void onSeriesChanged(Qt::CheckState seriesState, MPlotItem *series);

};

#endif // STRIPTOOLPLOT_H
