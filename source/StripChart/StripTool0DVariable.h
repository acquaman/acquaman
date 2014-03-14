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
