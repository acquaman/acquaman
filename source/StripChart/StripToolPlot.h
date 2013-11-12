#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QtGui>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotWidget.h"

#include "StripChart/StripToolModel.h"

/// This class displays pv information as a plot. A series may be added or removed from the plot, and displays the units of a series when it's selected.

class StripToolPlot : public MPlotWidget
{
    Q_OBJECT

public:
    explicit StripToolPlot(QWidget *parent = 0);
    ~StripToolPlot();
    
signals:
    void seriesSelected(MPlotItem *plotSelection, bool isSelected);
    void updatePlotSelection(MPlotItem *newSelection);

protected:
    StripToolModel *model_;
    MPlot *plot_;
    MPlotPlotSelectorTool *selector_;
    MPlotItem *plotSelection_;

public:
    void setModel(StripToolModel *model);

protected:
    /// Returns true if a given series is in the plot, false otherwise.
    bool contains(MPlotItem *series);
    /// Adds a given series to the plot, if it hasn't been added already.
    void addSeriesToPlot(MPlotItem *newSeries);
    /// Removes a given series from the plot, if it is contained in the plot.
    void removeSeriesFromPlot(MPlotItem *toRemove);
    
protected slots:
    /// An attempt at uniform selection--not fully implemented. The model's selectedPV_ becomes the selected series on the plot.
    void toSetPlotSelection(MPlotItem *modelSelection);
    /// Informs the model that a series has been selected.
    void onSeriesSelected(MPlotItem *plotSelection);
    /// Informs the model that a series has been deselected.
    void onSeriesDeselected();
    /// Causes the string arguments to become the labels on the plot.
    void setPlotAxesLabels(const QString &bottomLabel, const QString &leftLabel);
    void setPlotAxesRanges(const MPlotAxisRange &axisBottom);
    void setTicksVisible(bool isShown);
    /// Removes a series from the plot if the series is unchecked, adds a series if it is checked.
    void onSeriesChanged(Qt::CheckState seriesState, MPlotItem *series);

    void toUpdatePlotSelection(MPlotItem *newSelection);

};

#endif // STRIPTOOLPLOT_H
