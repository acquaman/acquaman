#include "StripToolSeries.h"

StripToolSeries::StripToolSeries(const MPlotAbstractSeriesData *data) :
    MPlotSeriesBasic()
{
    setDefaults();
    setModel(data);
}



StripToolSeries::~StripToolSeries()
{
}



void StripToolSeries::setPVName(const QString pvName)
{
    pvName_ = pvName;
}



QString StripToolSeries::pvName()
{
    return pvName_;
}



void StripToolSeries::setPVUnits(const QString pvUnits)
{
    pvUnits_ = pvUnits;
}



QString StripToolSeries::pvUnits()
{
    return pvUnits_;
}
