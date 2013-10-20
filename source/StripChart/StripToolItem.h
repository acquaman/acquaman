#ifndef STRIPTOOLITEM_H
#define STRIPTOOLITEM_H

#include "beamline/AMPVControl.h"
#include "MPlot/MPlotSeriesData.h"

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
    bool isUpdating_;
    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvDataTotal_;
    QVector<double> xValuesDisplayed_;
    QVector<double> yValuesDisplayed_;
    AMReadOnlyPVControl *pvControl_;
    MPlotVectorSeriesData *pvData_;

protected:
    QString pvName();
    QString pvDescription();
    QString xUnits();
    QString yUnits();
    MPlotVectorSeriesData* data();
    int valuesDisplayed();
    void setValuesDisplayed(const int newValuesDisplayed);
    void setPVUpdating(bool isUpdating);

protected slots:
    void onPVValueChanged(double newValue);
    
};

#endif // STRIPTOOLITEM_H
