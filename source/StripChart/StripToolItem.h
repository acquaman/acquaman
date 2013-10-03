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
    friend class StripToolContainer;
    
signals:
    void pvConnected(const QString &pvName);
    void pvDataUpdate(const QString &pvName, QVector<double>, QVector<double>);

protected:
    int updateIndex_;
    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    int valuesDisplayed_;
    AMReadOnlyPVControl *pvControl_;
    MPlotVectorSeriesData *pvDataDisplay_;
    MPlotSeriesBasic *pvSeries_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvDataTotal_;

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
