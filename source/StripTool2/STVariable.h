#ifndef STVARIABLE_H
#define STVARIABLE_H

#include <QObject>
#include <QDebug>

#include "beamline/AMProcessVariable.h"
#include "analysis/AM0DAccumulatorAB.h"
#include "analysis/AM0DTimestampAB.h"
#include "analysis/AM1DTimedDataAB.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

#include "MPlot/MPlotSeries.h"

class STVariable : public QObject
{
    Q_OBJECT

public:
    /// Constructor. Takes process variable name argument.
    explicit STVariable(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~STVariable();

    /// Returns the process variable.
    AMProcessVariable *pv() const;
    /// Returns the series tracking pv data updates.
    MPlotSeriesBasic *series() const;


signals:
    /// Emitted when the process variable is connected.
    void connected(bool isConnected);

public slots:
    /// Sets the number of data points and time points that are saved in total.
    void setDataBufferSize(int bufferSize);
    /// Sets the time window to be displayed, relative to now.
    void setTimeFilter(int interval, AM0DTimestampAB::TimeUnits units);

protected slots:

protected:
    /// The process variable that this class listens to for value updates.
    AMProcessVariable *pv_;

    /// Produces a list of value updates from the pv_.
    AM0DAccumulatorAB *data_;
    /// Produces a list of time updates from the pv_.
    AM0DTimestampAB *times_;
    /// Produces the values (y) and times (x) to be displayed at a given time.
    AM1DTimedDataAB *timedData_;

    AMDataSourceSeriesData *seriesData_;
    MPlotSeriesBasic *series_;

};

#endif // STVARIABLE_H
