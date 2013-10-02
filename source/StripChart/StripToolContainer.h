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
    QMap<QString, MPlotSeriesBasic *> pvNameToSeriesMap;

protected:
    void addItem(const QString &pvName, const QString &pvDescription);
    void removeItem(const QString &pvName);
    bool contains(const QString &pvName);
    void setValuesDisplayed(const QString &pvName, const int newValuesDisplayed);
    MPlotSeriesBasic* getSeries(const QString &pvName);
    
public slots:
    void onPVDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues);
    
};

#endif // STRIPTOOLCONTAINER_H
