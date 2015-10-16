#include "AM1DMaximumAB.h"
#include "util/AMUtility.h"

AM1DMaximumAB::AM1DMaximumAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	// Initialize member variables.

	data_ = 0;

	canAnalyze_ = false;
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();

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

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	return cachedData_.at(indexes.i());
}

bool AM1DMaximumAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 0 || indexEnd.rank() != 0)
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.i(), totalSize*sizeof(double));

	return true;
}

AMNumber AM1DMaximumAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return AMNumber(data_->axisValue(axisNumber, index));
}

bool AM1DMaximumAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	return (data_->axisValues(axisNumber, startIndex, endIndex, outputValues));
}

void AM1DMaximumAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	cacheUpdateRequired_ = true;
	emitValuesChanged(start, end);
}

void AM1DMaximumAB::onInputSourceSizeChanged()
{
	axes_[0].size = data_->size(0);
	cacheUpdateRequired_ = true;
	cachedData_ = QVector<double>(size(0));
	emitSizeChanged();
}

void AM1DMaximumAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DMaximumAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (data_) {
		disconnect( data_->signalSource(), 0, this, 0 );
		data_ = 0;
	}

	if (dataSources.isEmpty()) {
		data_ = 0;
		canAnalyze_ = false;
		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Source Maximum");

	} else if (dataSources.count() == 1) {
		data_ = dataSources.at(0);
		canAnalyze_ = true;
		sources_ = dataSources;
		axes_[0] = data_->axisInfoAt(0);

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("Maximum of %1").arg(data_->name()));

		connect( data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
		connect( data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()) );
		connect( data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
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
	if (data_ && data_->isValid())
		setState(0);
	else
		setState(AMDataSource::InvalidFlag);
}

void AM1DMaximumAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(0);
	AMnDIndex end = size() - 1;
	int totalSize = size(0);

	QVector<double> data = QVector<double>(totalSize);

	data_->values(start, end, data.data());

	for (int i = 0; i < totalSize; i++) {
		cachedData_[i] = data.at(i);
	}

	cachedDataRange_ = AMUtility::rangeFinder(cachedData_);
	cacheUpdateRequired_ = false;
}
