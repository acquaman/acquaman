/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

    /// Returns the pv name for this variable.
    QString name() const;

    /// Returns the description set for this variable, empty string if none was given.
    QString description() const;

    /// Returns this variable's creation datetime.
    QDateTime created() const;

    /// Returns the process variable.
    AMProcessVariable *pv() const;

    /// Returns true if this variable is connected, false otherwise.
    bool isConnected() const;

    /// Returns the variable's latest value.
    double value() const;

    /// Returns the series tracking pv data updates.
    MPlotSeriesBasic *series() const;


signals:
    /// Emitted when the description changes.
    void descriptionChanged(const QString &newDescription);
    /// Emitted when the variable's value has changed.
    void valueChanged(double newValue);
    /// Emitted when the process variable is connected.
    void connected(bool isConnected);

public slots:
    /// Sets the description for this variable.
    void setDescription(const QString &newDescription);
    /// Sets the number of data points and time points that are saved in total.
    void setDataBufferSize(int bufferSize);
    /// Sets the time window to be displayed, relative to now.
    void setTimeFilter(int interval, AM0DTimestampAB::TimeUnits units);

protected slots:

protected:

protected:
    /// The datetime for this variable's initial creation.
    QDateTime created_;

    /// The variable's description.
    QString description_;

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
