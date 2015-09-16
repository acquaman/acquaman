#include "AM1DMaximumAB.h"

AM1DMaximumAB::AM1DMaximumAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	// Initialize member variables.

	data_ = 0;

	canAnalyze_ = false;

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

	int totalSize = endIndex - startIndex + 1;
	QVector<double> axisData = QVector<double>(totalSize, 0);

	data_->axisValues(axisNumber, startIndex, endIndex, outputValues);

	return true;
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
