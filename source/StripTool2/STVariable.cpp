#include "STVariable.h"

STVariable::STVariable(const QString &name, QObject *parent) :
    QObject(parent)
{
    pv_ = new AMProcessVariable(name, true, this);

    data_ = new AM0DAccumulatorAB("PVData", this);
    data_->setDataStoredCountMax(50);
    data_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(pv_, pv_->pvName(), this));

    times_ = new AM0DTimestampAB("Timestamps", this);
    times_->setDataStoredCountMax(50);
    times_->setTimeUnits(AM0DTimestampAB::Seconds);
    times_->setTimeValue(10);
    times_->enableTimeFiltering(true);
    times_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(pv_, pv_->pvName(), this));

    timedData_ = new AM1DTimedDataAB("TimedData", this);
    timedData_->setInputDataSources(QList<AMDataSource*>() << data_ << times_);

    seriesData_ = new AMDataSourceSeriesData(timedData_);
    series_ = new MPlotSeriesBasic();
    series_->setModel(seriesData_, true);
    series_->setDescription(" ");

    connect( pv_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)) );
    if (pv_->isConnected())
        emit connected(true);

}

STVariable::~STVariable()
{

}

AMProcessVariable* STVariable::pv() const
{
    return pv_;
}

MPlotSeriesBasic* STVariable::series() const
{
    return series_;
}

void STVariable::setDataBufferSize(int bufferSize)
{
    data_->setDataStoredCountMax(bufferSize);
    times_->setDataStoredCountMax(bufferSize);
}

void STVariable::setTimeFilter(int interval, AM0DTimestampAB::TimeUnits units)
{
    times_->setTimeValue(interval);
    times_->setTimeUnits(units);
}
