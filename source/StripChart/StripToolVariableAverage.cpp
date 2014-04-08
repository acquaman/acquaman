#include "StripToolVariableAverage.h"

StripToolVariableAverage::StripToolVariableAverage(StripToolVariableInfo* info, QObject *parent) :
    StripToolVariable(info, parent)
{
//    defaultSettings();
}



//void StripToolVariableAverage::setBaseVariable(StripToolVariable *baseVariable)
//{
//    baseVariable_ = baseVariable;
//    qDebug() << "StripToolVariableAverage :: new base variable set.";
//}



//void StripToolVariableAverage::setBinSize(int binSize)
//{
//    if (binSize > 0) {
//        binSize_ = binSize;
//    } else {
//        qDebug() << "StripToolVariableAverage :: attempted to set bin size to a number that is <= 0. Setting bin size to 1.";
//        binSize_ = 1;
//    }
//}



//void StripToolVariableAverage::onDataSourceValuesChanged(QTime measurementTime, double measurementValue)
//{
//    updateTotalValues(measurementTime, measurementValue);
//    applyAverage(totalUpdateTimes_, totalUpdateValues_);

//    if (info()->checkState() == Qt::Checked)
//        updateDisplayValues();
//}



//void StripToolVariableAverage::applyAverage()
//{
//    AM1DRunningAverageFilterAB *averageAB = new AM1DRunningAverageFilterAB(binSize_, "avg-" + QString::number(binSize_) + " " + info()->name(), this);
//    averageAB->setInputDataSources(QList<AMDataSource*>() << baseVariable_->info()->dataSource());

//}



//void StripToolVariableAverage::defaultSettings()
//{
//    binSize_ = 1;
//    baseVariable_ = 0;
//}

