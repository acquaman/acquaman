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


//#ifndef STRIPTOOL0DVARIABLE_H
//#define STRIPTOOL0DVARIABLE_H

//#include <QObject>

//#include "MPlot/MPlotSeriesData.h"

//#include "StripChart/StripToolBasicVariable.h"
//#include "StripChart/StripTool0DVariableInfo.h"
//#include "StripChart/StripToolSeries.h"

//class StripTool0DVariable : public StripToolBasicVariable
//{
//    Q_OBJECT

//public:
//    explicit StripTool0DVariable(StripTool0DVariableInfo *info, AMDataSource *dataSource, QObject *parent = 0);

//signals:
//    void infoChanged();
//    void totalVectorsUpdated();
//    void dataRangeUpdated(MPlotAxisRange*);
//    void displayRangeUpdated(MPlotAxisRange*);

//public:
//    StripTool0DVariableInfo* info() const;
//    MPlotVectorSeriesData* data() const;
//    StripToolSeries* series() const;
//    MPlotItem* plotItem() const;
//    QString colorName() const;
//    int granularity() const;
////    MPlotAxisRange* displayRange() const;
////    MPlotAxisRange* dataRange() const;

//public slots:
//    void setColorName(const QString &newColor);
//    void setGranularity(int newGranularity);

//private slots:
//    void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);

//private:
//    void totalVectorSizeCheck();
//    void updateTotalValues(QTime latestTime, double latestValue);
//    void updateDisplayValues();

//private:
//    StripTool0DVariableInfo *info_;
//    MPlotVectorSeriesData *data_;
//    StripToolSeries *series_;

//    int updateCount_;
//    int displayedTime_; // the amount of time we want to see plotted. in milliseconds.
//    QVector<QTime> totalUpdateTimes_;
//    QVector<double> totalUpdateValues_;



//};

//#endif // STRIPTOOL0DVARIABLE_H
