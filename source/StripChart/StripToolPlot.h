#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QtGui>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"

#include "StripChart/StripToolSeries.h"

class StripToolPlot : public QObject
{
    Q_OBJECT

public:
    explicit StripToolPlot(QObject *parent = 0);
    ~StripToolPlot();
    friend class StripTool;
    
signals:

protected:
    MPlot *basePlot_;
    MPlotPlotSelectorTool *selector_;
    StripToolSeries *selectedItem_;
    QMap<QString, MPlotItem*> namesToShownSeries_;
    QMap<QString, MPlotItem*> namesToHiddenSeries_;

protected:
    MPlot* plot();
    QList<QString> getActivePVList();
    bool contains(const QString &pvName);
    bool pvShown(const QString &pvName);
    bool pvHidden(const QString &pvName);
    void addSeries(const QString &pvName, const QString &pvUnits, MPlotVectorSeriesData *pvData);
    void showSeries(const QString &pvName);
    void hideSeries(const QString &pvName);
    void deleteSeries(const QString &pvName);
    void showPlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel);
    void showPlotAxesScale(StripToolSeries *plotSelection);
    void showSeriesDescription(StripToolSeries *selectedItem);
    void hidePlotAxesLabels();
    void hideSeriesDescription(StripToolSeries *deselectedItem);
    
protected slots:
    void toShowCheckedPV(const QString &pvName);
    void toRemoveUncheckedPV(const QString &pvName);
    void showItemInfo(MPlotItem *selectedItem);
    void hideItemInfo();

};

#endif // STRIPTOOLPLOT_H
