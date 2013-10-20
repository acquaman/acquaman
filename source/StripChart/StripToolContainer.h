#ifndef STRIPTOOLCONTAINER_H
#define STRIPTOOLCONTAINER_H

#include "StripChart/StripToolItem.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotSeries.h"

class StripToolContainer : public QObject
{
    Q_OBJECT

public:
    explicit StripToolContainer(QObject *parent = 0);
    ~StripToolContainer();
    friend class StripTool;
    
signals:

protected:
    QMap<QString, StripToolItem *> pvNameToItemMap;

protected:
    bool contains(const QString &pvName);
    void addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void deleteItem(const QString &pvName);
    int valuesDisplayed(const QString &pvName);
    void setValuesDisplayed(const QString &pvName, const int newValuesDisplayed);
    void setPVUpdating(const QString &pvName, bool isUpdating);
    MPlotVectorSeriesData* data(const QString &pvName);
    QString description(const QString &pvName);
    QString xUnits(const QString &pvName);
    QString yUnits(const QString &pvName);
    QList<QString> getPVList();
};

#endif // STRIPTOOLCONTAINER_H
