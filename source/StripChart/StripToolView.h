#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>
#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolSidePanel.h"
#include "StripChart/ReloadPVDialog.h"
#include "StripChart/StripToolControlsPanel.h"

/// This class is the basic view for the application. The first (read: main) component is the plot, and the second is an instance of "quick controls" that can be shown or hidden using a checkbox. This class also checks whether reloading old pvs is a possibility and confirms with the user whether or not they want to.

class StripToolView : public QWidget
{
    Q_OBJECT
    
public:
    explicit StripToolView(QWidget *parent = 0, StripToolModel *model = 0);
    ~StripToolView();
    friend class StripTool;

signals:
    //    void reloadPVs(bool reload);
    void viewSelectionChange();
    void plotSelectionChanged(MPlotItem *newSelection);
    void listSelectionChanged(const QModelIndex &newSelection);

private:
    StripToolModel* model() const;
    StripToolPlot* plotView() const;
    StripToolSidePanel* sidePanel() const;
    StripToolControlsPanel* controlPanel() const;

    /// Creates instances of StripToolPlot, StripToolSidePanel, and StripToolControlsPanel.
    void buildUI();
    void setPlotLeftAxisName(const QString &newName);
    void setPlotBottomAxisName(const QString &newName);
    void setPlotSelection(MPlotItem *newSelection);
    MPlotItem* plotSelection();
    void addPlotItem(MPlotItem *newItem);
    void removePlotItem(MPlotItem *oldItem);

    void setListSelection(const QModelIndex &selectedIndex);
    QModelIndex listSelection() const;

private slots:
    void onSeriesDeselected();

private:
    StripToolModel *model_;
    StripToolPlot *plotView_;
    StripToolSidePanel *sidePanel_;
    StripToolControlsPanel *controlPanel_;

};

#endif // STRIPTOOLVIEW_H
