#ifndef STRIPTOOLITEM_H
#define STRIPTOOLITEM_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

class StripToolItem : public QObject
{
    Q_OBJECT

public:
    explicit StripToolItem(QString pvName, QString pvDescription = "", QObject *parent = 0);
    
signals:
    void pvDataUpdate();

protected:
    int updateIndex_;
    QString pvName_;
    QString pvDescription_;

    AMReadOnlyPVControl *pvControl_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvData_;

protected:
    QString getPVName();
    QString getPVDescription();
    QVector<double> getPVUpdateIndices();
    QVector<double> getPVData();
    
protected slots:
    void onPVValueChanged(double);
    
};

#endif // STRIPTOOLITEM_H
