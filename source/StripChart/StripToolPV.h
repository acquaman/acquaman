#ifndef STRIPTOOLPV_H
#define STRIPTOOLPV_H

#include <QObject>
#include <QDebug>

#include "beamline/AMPVControl.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

class StripToolPV : public QObject
{
    Q_OBJECT

public:
    StripToolPV(const QString &pvName, const QString &pvDescription = "", const QString &pvUnits = "", QObject *parent = 0);
    ~StripToolPV();
    friend class StripToolModel;

signals:

protected:
    int updateIndex_;
    int defaultValuesDisplayed_;
    int valuesDisplayed_;
    int dataVectorSize_;
    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    bool isUpdating_;
    Qt::CheckState checkState_;
    bool displayAll_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvDataTotal_;
    QVector<double> xValuesDisplayed_;
    QVector<double> yValuesDisplayed_;
    MPlotVectorSeriesData *pvData_;
    MPlotSeriesBasic *pvSeries_;
    AMReadOnlyPVControl *pvControl_;

protected:
    QString pvName();
    QString pvDescription();
    QString xUnits();
    QString yUnits();
    Qt::CheckState checkState();
    MPlotVectorSeriesData* data();
    MPlotSeriesBasic* series();
    int valuesDisplayed();

    void setDescription(const QString &newDescription);
    void setUnits(const QString &newUnits);
    void setPVUpdating(bool isUpdating);
    void setValuesDisplayed(int points);
    void incrementValuesDisplayed(int diff);
    void setAllValuesDisplayed(bool displayAll);
    void setCheckState(Qt::CheckState isChecked);

protected slots:
    void onPVValueChanged(double newValue);

};

#endif // STRIPTOOLPV_H
