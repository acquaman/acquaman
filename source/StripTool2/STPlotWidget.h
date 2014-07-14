#ifndef STPLOTWIDGET_H
#define STPLOTWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QDebug>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

#include "STTime.h"
#include "STPlotSelectorTool.h"

class STTime;

class STPlotWidget : public MPlotWidget
{
    Q_OBJECT

public:
    explicit STPlotWidget(QWidget *parent = 0);
    virtual ~STPlotWidget();

    QString plotName() const;
    bool plotNameVisible() const;
    bool timeFilteringEnabled() const;
    STTime *time() const;
    STPlotSelectorTool *selector() const;

signals:
    void plotNameChanged(const QString &name);
    void leftAxisNameChanged(const QString &name);
    void bottomAxisNameChanged(const QString &name);
    void timeChanged(STTime *time);
    void timeFilteringEnabled(bool isEnabled);
    void plotSelectionChanged(MPlotItem *selection);

public slots:
    void setPlotName(const QString &newName);
    void showPlotName(bool show);
    void enableTimeFiltering(bool enabled);
    void setLeftAxisName(const QString &newName);
    void setBottomAxisName(const QString &newName);

protected slots:
    void onTimeUnitsChanged(STTime::Units newUnits);
    void onPlotSelectionChanged();

protected:
    QString plotName_;
    bool plotNameVisible_;
    bool timeFilteringEnabled_;
    STTime *time_;
    STPlotSelectorTool *selector_;

};

#endif // STPLOTWIDGET_H
