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


#ifndef STRIPTOOLVARIABLE_H
#define STRIPTOOLVARIABLE_H

#include <QObject>

#include "MPlot/MPlotSeriesData.h"
#include "dataman/AMnDIndex.h"

#include "StripChart/StripToolSeries.h"
#include "StripChart/StripToolSeries2.h"
#include "StripChart/StripToolVariableInfo.h"

class MPlotAxisRange;

class StripToolVariable : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariable(StripToolVariableInfo *info = 0, QObject *parent = 0);

signals:
    void dataMaxUpdated(double newMax);
    void dataMinUpdated(double newMin);
    void displayRangeUpdated(const MPlotAxisRange* newRange);

public:
    QModelIndex index() const;
    StripToolVariableInfo* info() const;
    MPlotVectorSeriesData* data() const;
    StripToolSeries *series() const;

public slots:
    void setIndex(const QModelIndex &index);
    void setInfo(StripToolVariableInfo* newInfo);
    void setCustomAxisMax(double newMax);
    void setCustomAxisMin(double newMin);
    void eraseCustomAxisMax();
    void eraseCustomAxisMin();

protected slots:
    virtual void onDataSourceValuesChanged(QTime measurementTime, double measurementValue);
    void toUpdateSeriesColor(const QString &colorName);

protected:
    void updateTotalValues(QTime latestTime, double latestValue);
    void updateDisplayValues(QTime latestTime, QVector<QTime> totalTimes, QVector<double> totalValues);

private:
    void totalVectorSizeCheck();

    QVector<double> applyAverage(QVector<double> toAverage);
    QVector<double> applyGranularity(QVector<double> toGranulize);

    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    QModelIndex index_;
    StripToolVariableInfo* info_;
    MPlotVectorSeriesData* data_;
    StripToolSeries* series_;

    int updateCount_;
    QVector<QTime> totalUpdateTimes_;
    QVector<double> totalUpdateValues_;

};

#endif // STRIPTOOLVARIABLE_H
