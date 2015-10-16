#include "AM1DMaximumAB.h"
#include "util/AMUtility.h"
#include <QDebug>

AM1DMaximumAB::AM1DMaximumAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	// Initialize member variables.

	inputSource_ = 0;
	canAnalyze_ = false;
	cacheUpdateRequired_ = false;

	// Initialize inherited variables.

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

AM1DMaximumAB::~AM1DMaximumAB()
{

}

bool AM1DMaximumAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	bool result = false;

	// The null input is always acceptable.
	// Otherwise, we expect a single input source with rank 1.

	if (dataSources.isEmpty())
		result = true;
	else if (dataSources.count() == 1 && dataSources.at(0)->rank() == 1)
		result = true;

	return result;
}

AMNumber AM1DMaximumAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 0)
		return AMNumber(AMNumber::DimensionError);

	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (cacheUpdateRequired_)
		computeCachedValues();

	return AMNumber(maxValue_);
}

bool AM1DMaximumAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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

AMNumber AM1DMaximumAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index != 0)
		return AMNumber(AMNumber::OutOfBoundsError);

	return AMNumber(maxAxisValue_);
}

bool AM1DMaximumAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
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

void AM1DMaximumAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	cacheUpdateRequired_ = true;
	emitValuesChanged(AMnDIndex(), AMnDIndex());
}

void AM1DMaximumAB::onInputSourceStateChanged()
{
	reviewState();
}

void AM1DMaximumAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (inputSource_) {
		disconnect( inputSource_->signalSource(), 0, this, 0 );
		inputSource_ = 0;
	}

	if (dataSources.isEmpty()) {
		inputSource_ = 0;
		canAnalyze_ = false;
		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Source Maximum");

	} else if (dataSources.count() == 1) {
		inputSource_ = dataSources.at(0);
		canAnalyze_ = true;
		cacheUpdateRequired_ = true;
		sources_ = dataSources;
		axes_[0] = AMAxisInfo("valid", 1, "Maximum value");
		setDescription(QString("Maximum of %1").arg(inputSource_->name()));

		connect( inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
		connect( inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
	}

	reviewState();

	setModified(true);

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM1DMaximumAB::reviewState()
{
	if (inputSource_ && inputSource_->isValid())
		setState(0);
	else
		setState(AMDataSource::InvalidFlag);
}

void AM1DMaximumAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(0);
	AMnDIndex end = inputSource_->size() - 1;
	int totalSize = inputSource_->size(0);

	// Copy the input source's data.

	QVector<double> data = QVector<double>(totalSize);
	inputSource_->values(start, end, data.data());

	QVector<double> axisData = QVector<double>(totalSize);
	inputSource_->axisValues(0, start.i(), end.i(), axisData.data());

	// Initialize max value and max axis value.

	maxValue_ = data.at(0);
	maxAxisValue_ = axisData.at(0);

	// Iterate through the data to find the largest value.

	for (int i = 1, count = data.count(); i < count; i++) {
		double value = data.at(i);

		if (value > maxValue_) {
			maxValue_ = value;
			maxAxisValue_ = axisData.at(i);
		}
	}

	cacheUpdateRequired_ = false;
}
