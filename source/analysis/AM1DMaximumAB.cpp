#include "AM1DMaximumAB.h"

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
	if (indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (indexes.i() < 0 || indexes.i() >= cachedData_.size())
		return AMNumber(AMNumber::InvalidError);

	if (cacheUpdateRequired_)
		computeCachedValues();

	return cachedData_.at(indexes.i());
}

bool AM1DMaximumAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData() + indexStart.i(), totalSize * sizeof(double));

	return true;
}

AMNumber AM1DMaximumAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return inputSource_->axisValue(axisNumber, index);
}

bool AM1DMaximumAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return inputSource_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM1DMaximumAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	cacheUpdateRequired_ = true;
	emitValuesChanged(start, end);
}

void AM1DMaximumAB::onInputSourceSizeChanged()
{
	axes_[0].size = inputSource_->size(0);
	cacheUpdateRequired_ = true;
	cachedData_ = QVector<double>(axes_.at(0).size);
	emitSizeChanged();
}

void AM1DMaximumAB::onInputSourceStateChanged() {

	// Check the size, just in case the size has changed while the input source was invalid, and now it's going valid.
	onInputSourceSizeChanged();

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
		setDescription("1D Maximum Data Source");

	} else if (dataSources.count() == 1) {
		inputSource_ = dataSources.at(0);
		canAnalyze_ = true;
		cacheUpdateRequired_ = true;
		sources_ = dataSources;
		axes_[0] = AMAxisInfo("valid", 1, "Maximum value");
		setDescription(QString("Maximum of %1").arg(inputSource_->name()));

		connect( inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
		connect( inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()) );
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
	if (canAnalyze_ && inputSource_ && inputSource_->isValid())
		setState(0);
	else
		setState(AMDataSource::InvalidFlag);
}

void AM1DMaximumAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(0);
	AMnDIndex end = size() - 1;
	int totalSize = size(0);

	// Copy the input source's data.

	QVector<double> data = QVector<double>(totalSize);
	inputSource_->values(start, end, data.data());

	// Initialize max value with first data source value.

	double maxValue = data.at(0);

	// Iterate through the source's data, comparing values.

	for (int i = 1; i < totalSize; i++) {
		double sourceValue = data.at(i);

		if (sourceValue > maxValue)
			maxValue = sourceValue;

		cachedData_[i] = maxValue;
	}

	cacheUpdateRequired_ = false;
}

