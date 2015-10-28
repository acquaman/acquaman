#include "AM0DMaximumAB.h"
#include "util/AMUtility.h"
#include <QDebug>

AM0DMaximumAB::AM0DMaximumAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	// Initialize member variables.

	maximumAB_ = new AM1DMaximumAB("InvalidInput", this);

	connect( maximumAB_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
	connect( maximumAB_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );

	cacheUpdateRequired_ = false;

	// Initialize inherited variables.

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

AM0DMaximumAB::~AM0DMaximumAB()
{

}

bool AM0DMaximumAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	return maximumAB_->areInputDataSourcesAcceptable(dataSources);
}

bool AM0DMaximumAB::canAnalyze() const
{
	return maximumAB_->canAnalyze();
}

AMNumber AM0DMaximumAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 0)
		return AMNumber(AMNumber::DimensionError);

	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (cacheUpdateRequired_)
		computeCachedValues();

	return AMNumber(maxValue_);
}

bool AM0DMaximumAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 0 || indexEnd.rank() != 0)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

	int totalSize = indexStart.totalPointsTo(indexEnd);

	if (totalSize != 1)
		return false;

	if (cacheUpdateRequired_)
		computeCachedValues();

	outputValues[0] = maxValue_;

	return true;
}

AMNumber AM0DMaximumAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index != 0)
		return AMNumber(AMNumber::OutOfBoundsError);

	return AMNumber(maxAxisValue_);
}

bool AM0DMaximumAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex != 0 || endIndex != 0)
		return false;

	outputValues[0] = maxAxisValue_;

	return true;
}

void AM0DMaximumAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	cacheUpdateRequired_ = true;
	emitValuesChanged(AMnDIndex(), AMnDIndex());
}

void AM0DMaximumAB::onInputSourceStateChanged()
{
	reviewState();
}

void AM0DMaximumAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	maximumAB_->setInputDataSources(dataSources);

	reviewState();

	setModified(true);

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM0DMaximumAB::reviewState()
{
	setState(maximumAB_->state());
}

void AM0DMaximumAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(0);
	AMnDIndex end = maximumAB_->size() - 1;
	int totalSize = maximumAB_->size(0);

	// Copy the input source's data.

	QVector<double> data = QVector<double>(totalSize);
	maximumAB_->values(start, end, data.data());

	QVector<double> axisData = QVector<double>(totalSize);
	maximumAB_->axisValues(0, start.i(), end.i(), axisData.data());

	// Initialize max value and max axis value.

	maxValue_ = data.at(0);
	maxAxisValue_ = axisData.at(0);

	// Iterate through the data to find the largest value and corresponding axis value.

	for (int i = 1, count = data.count(); i < count; i++) {
		double value = data.at(i);

		if (value > maxValue_) {
			maxValue_ = value;
			maxAxisValue_ = axisData.at(i);
		}
	}

	cacheUpdateRequired_ = false;
}
