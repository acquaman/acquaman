#include "StripToolVariable.h"

#include <QVector>

StripToolVariable::StripToolVariable(StripToolVariableInfo *info, QObject *parent) :
    QObject(parent)
{
    info_ = 0;
    data_ = 0;
    series_ = 0;

    setInfo(info);

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolVariable object created.";
}



QModelIndex StripToolVariable::index() const
{
    return index_;
}



StripToolVariableInfo* StripToolVariable::info() const
{
    return info_;
}



MPlotVectorSeriesData* StripToolVariable::data() const
{
    return data_;
}



StripToolSeries *StripToolVariable::series() const
{
    return series_;
}



//MPlotSeriesBasic* StripToolVariable::plotItem() const
//{
//    return series_;
//}



void StripToolVariable::setIndex(const QModelIndex &index)
{
    if (index_ != QModelIndex() && index == QModelIndex()) {
        qDebug() << "StripToolVariable::setIndex(QModelIndex) : cannot overwrite a valid index with an invalid one. No change made.";
        return;
    }

    index_ = index;
}



void StripToolVariable::setInfo(StripToolVariableInfo *newInfo)
{
    if (info_ != 0) {
        disconnect( info_, SIGNAL(dataSourceValueUpdate(QTime, double)), this, SLOT(onDataSourceValuesChanged(QTime, double)) );
        disconnect( info_, SIGNAL(colorChanged(QString)), this, SLOT(toUpdateSeriesColor(QString)) );

        disconnect( info_, SIGNAL(axisMaxChanged(double)), this, SLOT(setCustomAxisMax(double)) );
        disconnect( info_, SIGNAL(axisMinChanged(double)), this, SLOT(setCustomAxisMin(double)) );
        disconnect( info_, SIGNAL(axisMaxErased()), this, SLOT(eraseCustomAxisMax()) );
        disconnect( info_, SIGNAL(axisMinErased()), this, SLOT(eraseCustomAxisMin()) );

        disconnect( this, SIGNAL(dataMaxUpdated(double)), info_, SIGNAL(dataMaxChanged(double)) );
        disconnect( this, SIGNAL(dataMinUpdated(double)), info_, SIGNAL(dataMinChanged(double)) );

        info_->deleteLater();
        info_ = 0;
    }

    info_ = newInfo;
    info_->setParent(this);

    if (info_ != 0) {
        connect( info_, SIGNAL(dataSourceValueUpdate(QTime, double)), this, SLOT(onDataSourceValuesChanged(QTime, double)) );
        connect( info_, SIGNAL(colorChanged(QString)), this, SLOT(toUpdateSeriesColor(QString)) );

        connect( info_, SIGNAL(axisMaxChanged(double)), this, SLOT(setCustomAxisMax(double)) );
        connect( info_, SIGNAL(axisMinChanged(double)), this, SLOT(setCustomAxisMin(double)) );
        connect( info_, SIGNAL(axisMaxErased()), this, SLOT(eraseCustomAxisMax()) );
        connect( info_, SIGNAL(axisMinErased()), this, SLOT(eraseCustomAxisMin()) );

        connect( this, SIGNAL(dataMaxUpdated(double)), info_, SIGNAL(dataMaxChanged(double)) );
        connect( this, SIGNAL(dataMinUpdated(double)), info_, SIGNAL(dataMinChanged(double)) );
    }

    qDebug() << "StripToolVariable :: new info set for variable " << info()->name();
}



void StripToolVariable::setCustomAxisMax(double newMax)
{
    series_->setCustomMax(newMax);
}



void StripToolVariable::setCustomAxisMin(double newMin)
{
    series_->setCustomMin(newMin);
}



void StripToolVariable::eraseCustomAxisMax()
{
    series_->eraseCustomMax();
}



void StripToolVariable::eraseCustomAxisMin()
{
    series_->eraseCustomMin();
}



void StripToolVariable::onDataSourceValuesChanged(QTime measurementTime, double measurementValue)
{
    updateTotalValues(measurementTime, measurementValue);

    if (info()->checkState() == Qt::Checked)
        updateDisplayValues(measurementTime, totalUpdateTimes_, totalUpdateValues_);
}



void StripToolVariable::toUpdateSeriesColor(const QString &colorName)
{
    series_->setLinePen( QPen(QColor(colorName)) );
}



void StripToolVariable::totalVectorSizeCheck()
{
    int timesSize = totalUpdateTimes_.size();
    int valuesSize = totalUpdateValues_.size();
    int delta = 100;

    // if the arrays happen to be of different sizes (not sure how this would happen... but just in case), we can fix it.

    if (timesSize != valuesSize) {
        qDebug() << "The time and value arrays have become different sizes. Resizing the smaller to match the size of the larger.";

        if (timesSize > valuesSize) {
            totalUpdateValues_.resize(timesSize);
            qDebug() << "Value array adjusted.";

        } else {
            totalUpdateTimes_.resize(valuesSize);
            qDebug() << "Time array adjusted.";
        }
    }

    // we assume now the arrays are of the same length.

    if (timesSize < updateCount_ + 1) {
        qDebug() << "Resizing time/data vectors for" << info()->name();
        totalUpdateTimes_.resize(timesSize + delta);
        totalUpdateValues_.resize(valuesSize + delta);
    }
}



void StripToolVariable::updateTotalValues(QTime latestTime, double latestValue)
{
    updateCount_++;

    // check that the totalUpdateTimes_/Values_ vectors are sufficiently large to accomodate a new value, resize if not.
    totalVectorSizeCheck();

    // we assume now the vectors are large enough. Add new values!
    totalUpdateTimes_[updateCount_ - 1] = latestTime;
    totalUpdateValues_[updateCount_ - 1] = latestValue;

    emit dataMinUpdated(series()->dataRange()->min());
    emit dataMaxUpdated(series()->dataRange()->max());
}



void StripToolVariable::updateDisplayValues(QTime latestTime, QVector<QTime> totalTimes, QVector<double> totalValues)
{
    int updateCount = totalTimes.size();

    if (updateCount > 0 && totalTimes.size() == totalValues.size()) {
        // we generate two new vectors to house the subset of values that will be displayed.
        QVector<double> displayedTimes(0);
        QVector<double> displayedValues(0);
        bool arraysComplete = false;

        int startIndex = updateCount - 1;
        int index = startIndex;

        while (index >= 0 && index < updateCount && !arraysComplete) {
            QTime measurementTime = totalTimes.at(index);
            double measurement = totalValues.at(index);

            // we want to ignore 'filler' values that don't represent actual measurements.
            if (measurementTime.isValid()) {

                double displayTimeMS = latestTime.msecsTo(measurementTime);

                // if the display time falls within the range we'd like to see on the plot, add the time and value to the display vectors.
                if (qAbs(displayTimeMS) <= qAbs(info()->displayedTimeMS())) {
                    displayedTimes.prepend(info()->timeMSToTimeUnits(displayTimeMS)); // convert the millisecond time to amount considering current units.
                    displayedValues.prepend(measurement);

                } else {
                    // we want to stop considering values that are outside the display range.
                    arraysComplete = true;
                }
            }

            index--;
        }

        if (updateCount%info()->granularity() == 0) {
            // update the series values plotted on the graph.
            data()->setValues(displayedTimes, displayedValues);

            // we check that the display range values for this variable make sense, just in case.
            qreal max = series()->displayedRange()->max();
            qreal min = series()->displayedRange()->min();

            if (max == min) {

                if (min == 0) {
                    qDebug() << "StripToolVariable :: the max and min values of the displayed range retrieved from StripToolSeries are both equal to zero! Setting each +/- 2.";
                    max = 2;
                    min = -2;
                    series()->setCustomLimits(min, max);

                } else {
                    qDebug() << "StripToolVariable :: the max and min values of the displayed range retrieved from StripToolSeries are identical. Scaling each by +/- 5%.";
                    min *= 0.95;
                    max *= 1.05;
                    series()->setCustomLimits(min, max);
                }
            }

            MPlotAxisRange *displayRange = series()->displayedRange();
            qDebug() << "StripToolVariable :: display range max : " << displayRange->max();

            // emit signal containing the range used for the y axis.
            emit displayRangeUpdated(displayRange);
        }
    }
}



QVector<double> StripToolVariable::applyAverage(QVector<double> toAverage)
{
    int binSize = info()->averageOverPoints();

    // the group size of the points to average should never be less than 1. In case it has happened accidentally, we can fix it.
    if (binSize < 1) {
        qDebug() << "StripToolVariable :: cannot average points together in groups less than one. Setting group size to 1.";
        info()->setAverageOverPoints(1);
        return QVector<double>();
    }

    // if the group size of the points to average together is 1, there is no real averaging happening! Return original array.
    if (binSize == 1)
        return toAverage;

    // if we don't have enough points yet to fill one "bin", return an empty array.
    if (toAverage.size() < binSize)
        return QVector<double>();

    // the number of points in the final, averaged array is going to be a function of how many points we have and the size of each bin.
    int binNum = toAverage.size() / binSize;

    QVector<double> averagedValues = QVector<double>(binNum, 0.0);

    for (int averagedCount = 0; averagedCount < binNum; averagedCount++) {
        qDebug() << "StripToolVariable :: beginning calculation of average for bin number : " << averagedCount;

        double binSum = 0;

        for (int toAverageCount = averagedCount*binSize; toAverageCount < averagedCount*binSize + binSize; toAverageCount++) {
            binSum += toAverage.at(toAverageCount);
        }

        double binValue = binSum * 1.0 / binSize;
        averagedValues[averagedCount] = binValue;

        qDebug() << "StripToolVariable :: bin value : " << binValue;
    }

    return averagedValues;

}



QVector<double> StripToolVariable::applyGranularity(QVector<double> toGranulize)
{
    int initialSize = toGranulize.size();
    int granSize = info()->granularity();
    int overage = initialSize%granSize;

    if (overage == 0)
        return toGranulize;
    else
        return toGranulize.mid(0, initialSize - overage);
}



void StripToolVariable::buildComponents()
{
    data_ = new MPlotVectorSeriesData();

    series_ = new StripToolSeries();

    series_->setModel(data_, true);
    series_->setDescription(" ");
    series_->setMarker(MPlotMarkerShape::None);

    if (info()->hasColor())
        series_->setLinePen( QPen(info()->color()) );
    else
        info()->setColor(QColor(Qt::red));

    if (info_->hasCustomAxisMax())
        series_->setCustomMax(info_->customAxisMax());

    if (info_->hasCustomAxisMin())
        series_->setCustomMin(info_->customAxisMin());

}



void StripToolVariable::makeConnections()
{
    // info connections made in 'setInfo'
}



void StripToolVariable::defaultSettings()
{
    index_ = QModelIndex();
    updateCount_ = 0;
    info()->setCreationDateTime(QDateTime::currentDateTime().toString());
}
