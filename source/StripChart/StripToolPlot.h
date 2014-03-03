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
    ~StripToolPlot();
    
signals:
    void addSeries(const QModelIndex &parent, int rowStart, int rowFinish);
    void removeSeries(const QModelIndex &parent, int rowStart, int rowFinish);
    void seriesSelected(MPlotItem *plotSelection);
    void seriesDeselected();
    void setPlotSelection(MPlotItem *modelSelection);

protected:
    StripToolModel *model_;
    MPlot *plot_;
    StripToolSelector *selector_;

    QAction *toggleControls_;

public:
    void setModel(StripToolModel *model);

protected:
    /// Returns true if a given series is in the plot, false otherwise.
    bool contains(MPlotItem *series);
    /// Adds a given series to the plot, if it hasn't been added already. Returns true if the unique series was successfully added.
    bool addSeriesToPlot(MPlotItem *newSeries);
    /// Removes a given series from the plot, if it is contained in the plot. Returns true if all instances of the series were removed.
    bool removeSeriesFromPlot(MPlotItem *toRemove);

    void createActions();
    
protected slots:
    /// Adds series to the plot when the rowsInserted() signal is emitted from the model.
    void toAddSeries(const QModelIndex &parent, int rowStart, int rowFinish);
    /// Removes series from the plot when the rowsAboutToBeRemoved() signal is emitted from the model.
    void toRemoveSeries(const QModelIndex &parent, int rowStart, int rowFinish);
    /// Causes the string arguments to become the labels on the plot.
    void setPlotAxesLabels(const QString &bottomLabel, const QString &leftLabel);
    void setPlotAxesRanges(const MPlotAxisRange &axisBottom);
    void setTicksVisible(bool isShown);
    /// Removes a series from the plot if the series is unchecked, adds a series if it is checked.
    void onSeriesChanged(Qt::CheckState seriesState, int rowChanged);
    /// Causes a series to become de/selected to match the model's selected pv.
    void onModelSelectionChange();

    void contextMenuEvent(QContextMenuEvent *event);
    void toToggleControls();

};

#endif // STRIPTOOLPLOT_H
