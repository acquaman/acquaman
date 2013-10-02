#ifndef STRIPTOOLCONTAINER_H
#define STRIPTOOLCONTAINER_H

#include <QObject>

#include "StripChart/StripToolItem.h"

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
    QMap<QString, MPlotVectorSeriesData *> pvNameToDataMap;
    QMap<QString, MPlotItem *> pvNameToSeriesMap;

protected:
    void addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits);
    void removeItem(const QString &pvName);
    bool contains(const QString &pvName);
    void setValuesDisplayed(const QString &pvName, const int newValuesDisplayed);
    MPlotItem* getSeries(const QString &pvName);
    QString getAxisLeft(MPlotItem *plotSelection);
    QString getAxisBottom(MPlotItem *plotSelection);
    
public slots:
    void onPVDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues);
    
};

#endif // STRIPTOOLCONTAINER_H
