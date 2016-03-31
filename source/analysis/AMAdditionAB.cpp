#include "AMAdditionAB.h"

#include "util/AMUtility.h"

AMAdditionAB::AMAdditionAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();
	setState(AMDataSource::InvalidFlag);
}

AMAdditionAB::~AMAdditionAB()
{

}

bool AMAdditionAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	int firstRank = dataSources.first()->rank();

	// otherwise input sources have the same rank.
	for (int i = 1; i < dataSources.count(); i++)
		if (dataSources.at(i)->rank() != firstRank)
			return false;

	return true;
}

AMNumber AMAdditionAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != rank())
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = indexes.rank(); i < size; i++)
		if((unsigned)indexes.at(i) >= (unsigned)axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

    if (cacheUpdateRequired_)
	computeCachedValues();

    return cachedData_.at(indexes.flatIndexInArrayOfSize(size()));
}

bool AMAdditionAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != rank() || indexEnd.rank() != rank())
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = indexStart.rank(); i < size; i++)
		if((unsigned)indexStart.at(i) >= (unsigned)axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.flatIndexInArrayOfSize(size()), totalSize*sizeof(double));

	return true;
}

AMNumber AMAdditionAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber < 0 && axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return sources_.at(0)->axisValue(axisNumber, index);
}

bool AMAdditionAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber < 0 && axisNumber >= rank())
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return sources_.at(0)->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AMAdditionAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	cacheUpdateRequired_ = true;

	AMnDIndex scanStart = start;
	AMnDIndex scanEnd = end;
	scanStart.setRank(start.rank()-1);
	scanEnd.setRank(end.rank()-1);

//	if (scanStart == scanEnd)
//		dirtyIndices_ << start;

	emitValuesChanged(start, end);
}

void AMAdditionAB::onInputSourceSizeChanged()
{
	for (int i = 0, size = rank(); i < size; i++)
		axes_[i].size = sources_.at(0)->size(i);

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	cachedData_ = QVector<double>(size().product());
	emitSizeChanged();
}

void AMAdditionAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AMAdditionAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(!sources_.isEmpty()) {

		for (int i = 0; i < sources_.size(); i++){

			disconnect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			disconnect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			disconnect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_.clear();

		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		sources_ = dataSources;
		axes_.clear();


		for (int i = 0, size = sources_.first()->rank(); i < size; i++)
			axes_ << sources_.at(0)->axisInfoAt(i);

		cacheUpdateRequired_ = true;
		dirtyIndices_.clear();
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("Sum of spectra from %1 maps").arg(sources_.size()));

		for (int i = 0; i < sources_.size(); i++){

			connect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			connect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			connect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged(AMnDIndex(rank(), AMnDIndex::DoInit));
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMAdditionAB::reviewState()
{
	// Are there data sources?
	if(sources_.isEmpty()){

		setState(AMDataSource::InvalidFlag);
		return;
	}

	// Are all the data sources the same size?
	AMnDIndex firstSize = sources_.first()->size();

	for (int i = 0, size = firstSize.rank(); i < size; i++)
		foreach (AMDataSource *dataSource, sources_)
			if(firstSize.at(i) != dataSource->size(i)){

				setState(AMDataSource::InvalidFlag);
				return;
			}

	// Validity check on all data sources.
	bool valid = true;

	for (int i = 0; i < sources_.size(); i++)
		valid = valid && sources_.at(i)->isValid();

	if (valid)
		setState(0);
	else
		setState(AMDataSource::InvalidFlag);
}

void AMAdditionAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex();
	AMnDIndex end = AMnDIndex();

	if (dirtyIndices_.isEmpty()){

		start = AMnDIndex(rank(), AMnDIndex::DoInit);
		end = size()-1;
	}

	else {

		start = dirtyIndices_.first();
		end = dirtyIndices_.last();
		end[rank()-1] = size(rank()-1)-1;
	}

	int totalSize = start.totalPointsTo(end);
	int flatStartIndex = start.flatIndexInArrayOfSize(size());

	QVector<double> data = QVector<double>(totalSize);
	sources_.at(0)->values(start, end, data.data());

	// Do the first data source separately to initialize the values.
	memcpy(cachedData_.data()+flatStartIndex, data.constData(), totalSize*sizeof(double));

	// Iterate through the rest of the sources.
	for (int i = 1, count = sources_.size(); i < count; i++){

		sources_.at(i)->values(start, end, data.data());

		for (int j = 0; j < totalSize; j++)
			cachedData_[flatStartIndex+j] += data.at(j);
	}

	if (dirtyIndices_.isEmpty())
		cachedDataRange_ = AMUtility::rangeFinder(cachedData_);

	else{
		AMRange cachedRange = AMUtility::rangeFinder(cachedData_.mid(flatStartIndex, totalSize));

		if (cachedDataRange_.minimum() > cachedRange.minimum())
			cachedDataRange_.setMinimum(cachedRange.minimum());

		if (cachedDataRange_.maximum() < cachedRange.maximum())
			cachedDataRange_.setMaximum(cachedRange.maximum());
	}

	cacheUpdateRequired_ = false;
	dirtyIndices_.clear();
}
