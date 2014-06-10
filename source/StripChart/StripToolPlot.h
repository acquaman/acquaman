#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QtGui>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotWidget.h"

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolSelector.h"

/// This class displays pv information as a plot. A series may be added or removed from the plot, and displays the units of a series when it's selected.

class StripToolPlot : public MPlotWidget
{
    Q_OBJECT

public:
    explicit StripToolPlot(QWidget *parent = 0);
    virtual ~StripToolPlot();
    
signals:
    void selectionChanged(MPlotItem *plotSelection);

public:
    bool contains(MPlotItem *series);
    MPlotItem* selectedItem() const;

public slots:
    bool addPlotItem(MPlotItem *newSeries);
    bool removePlotItem(MPlotItem *toRemove);
    void setSelectedItem(MPlotItem *newSelection);
    void setLeftAxisName(const QString &newName);
    void setLeftAxisRange(const MPlotAxisRange *newAxisRange);
    void setBottomAxisName(const QString &newName);
    void setWaterfall(bool waterfallOn);

protected slots:
    void onSelectionChange();

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    MPlot *plot_;
    StripToolSelector *selector_;
    bool waterfallOn_;

};

#endif // STRIPTOOLPLOT_H
