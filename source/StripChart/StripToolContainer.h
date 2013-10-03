#ifndef STRIPTOOLCONTAINER_H
#define STRIPTOOLCONTAINER_H

#include <QObject>

#include "StripChart/StripToolItem.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotSeries.h"

class StripToolContainer : public QObject
{
    Q_OBJECT

public:
    explicit StripToolContainer(QObject *parent = 0);
    friend class StripTool;
    
signals:

protected:
    int itemCount_;
    QMap<QString, StripToolItem *> pvNameToItemMap;

protected:
    void addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void deleteItem(const QString &pvName);
    bool contains(const QString &pvName);
    void setValuesDisplayed(const QString &pvName, const int newValuesDisplayed);
    MPlotItem* getSeries(const QString &pvName);
    QString getAxisLeft(MPlotItem *plotSelection);
    QString getAxisBottom(MPlotItem *plotSelection);
};

#endif // STRIPTOOLCONTAINER_H
