#ifndef STRIPTOOLITEM_H
#define STRIPTOOLITEM_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotMarker.h"

class StripToolItem : public QObject
{
    Q_OBJECT

public:
    explicit StripToolItem(QString pvName, QString pvDescription = "", QString pvUnits = "", QObject *parent = 0);
    ~StripToolItem();
    friend class StripToolContainer;
    
signals:

protected:
    int updateIndex_;
    int valuesDisplayed_;
    int dataVectorSize_;
    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvDataTotal_;
    AMReadOnlyPVControl *pvControl_;
    MPlotVectorSeriesData *pvDataDisplay_;
    MPlotSeriesBasic *pvSeries_;

protected:
    QString pvName();
    QString pvDescription();
    QString xUnits();
    QString yUnits();
    MPlotItem* series();
    void setValuesDisplayed(const int newValuesDisplayed);

protected slots:
    void onPVValueChanged(double newValue);
    
};

#endif // STRIPTOOLITEM_H
