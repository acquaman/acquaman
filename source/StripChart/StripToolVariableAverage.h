#ifndef STRIPTOOLVARIABLEAVERAGE_H
#define STRIPTOOLVARIABLEAVERAGE_H

#include "source/analysis/AM1DRunningAverageFilterAB.h"

#include "StripChart/StripToolVariable.h"

class StripToolVariableAverage : public StripToolVariable
{

public:
    StripToolVariableAverage(StripToolVariableInfo *info, QObject *parent = 0);

//public slots:
//    void setBaseVariable(StripToolVariable* baseVariable);
//    void setBinSize(int binSize);

//private:
//    void onDataSourceValuesChanged(QTime measurementTime, double measurementValue);
//    void applyAverage();
//    void defaultSettings();

//private:
//    int binSize_;
//    StripToolVariable* baseVariable_;
//    QVector<double> avgTimes_;
//    QVector<double> avgValues_;
};

#endif // STRIPTOOLVARIABLEAVERAGE_H
