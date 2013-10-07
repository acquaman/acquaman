#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QObject>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"

class StripToolPlot : public QObject
{
    Q_OBJECT

public:
    explicit StripToolPlot(QObject *parent = 0);
    ~StripToolPlot();
    friend class StripTool;
    
signals:
    void showItemValuesDisplayed(const QString pvName);

private:
    MPlot *basePlot_;
    MPlotPlotSelectorTool *selector_;
    QMap<QString, MPlotItem*> pvNameToItemMap_;

private:
    MPlot* plot();
    void addItem(const QString &pvName, MPlotItem *newItem);
    void removeItem(const QString &pvName, MPlotItem *removeItem);
    void showItemInfo(MPlotItem *selectedItem);
    void updatePlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel);
    void updatePlotAxesScale(MPlotItem *plotSelection);
    
private slots:
    
};

#endif // STRIPTOOLPLOT_H
