#ifndef STRIPTOOLSERIES_H
#define STRIPTOOLSERIES_H

#include <QObject>

#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"

class StripToolSeries : public MPlotSeriesBasic
{

public:
    explicit StripToolSeries(const MPlotAbstractSeriesData *data = 0);
    ~StripToolSeries();
    friend class StripToolPlot;

signals:

protected:
    QString pvName_;
    QString pvUnits_;

protected:
    QString pvName();
    QString pvUnits();
    void setPVName(const QString pvName);
    void setPVUnits(const QString pvUnits);

public slots:
    
};

#endif // STRIPTOOLSERIES_H
