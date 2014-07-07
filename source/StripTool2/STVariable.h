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

#include "STTime.h"

class STVariable : public QObject
{
    Q_OBJECT

public:
    /// Constructor. Takes process variable name argument.
    explicit STVariable(const QString &name, QObject *parent = 0);
    /// Destructor.
    virtual ~STVariable();

    /// Returns the variable's index, -1 if none has been provided.
    int index() const;

    /// Returns the pv name for this variable.
    QString name() const;

    /// Returns the description set for this variable, empty string if none are given.
    QString description() const;
    bool hasDescription() const;

    /// Returns the units set for this variable, empty string if none are given.
    QString units() const;
    bool hasUnits() const;

    /// Returns the color for this variable, red by default.
    QColor color() const;

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

    bool operator==(const STVariable &other) const;


signals:
    /// Emitted when the variable's index has changed.
    void indexChanged(int newIndex);
    /// Emitted when the description changes.
    void descriptionChanged(const QString &newDescription);
    /// Emitted when the units change.
    void unitsChanged(const QString &newUnits);
    /// Emitted when the color changes.
    void colorChanged(QColor newColor);
    /// Emitted when the variable's value has changed.
    void valueChanged(double newValue);
    /// Emitted when the process variable is connected.
    void connected(bool isConnected);

public slots:
    /// Sets the variable's index.
    void setIndex(int newIndex);
    /// Sets the description for this variable.
    void setDescription(const QString &newDescription);
    /// Sets the units for this variable.
    void setUnits(const QString &units);
    /// Sets the series color for this variable.
    void setColor(const QColor &newColor);
    /// Sets the number of data points and time points that are saved in total.
    void setDataBufferSize(int bufferSize);
    /// Sets the time window to be displayed, relative to now.
    void setTimeValue(int newValue);
    void setTimeUnits(STTime::Units newUnits);

    void setTimeFilteringEnabled(bool isEnabled);

protected slots:

protected:

protected:
    /// The variable's index.
    int index_;

    /// The datetime for this variable's initial creation.
    QDateTime created_;

    /// The variable's description.
    QString description_;
    bool hasDescription_;

    /// The variable's units.
    QString units_;
    bool hasUnits_;

    /// The series color.
    QColor color_;

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
