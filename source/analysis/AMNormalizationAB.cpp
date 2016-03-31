#include "AMNormalizationAB.h"

#include "util/AMUtility.h"

AMNormalizationAB::AMNormalizationAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	normalizer_ = 0;
	canAnalyze_ = false;
	dataName_ = "";
	normalizationName_ = "";
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();
	setState(AMDataSource::InvalidFlag);
}

AMNormalizationAB::~AMNormalizationAB()
{

}

bool AMNormalizationAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
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

void AMNormalizationAB::setDataName(const QString &name)
{
	dataName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
}

void AMNormalizationAB::setNormalizationName(const QString &name)
{
	normalizationName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
}

bool AMNormalizationAB::canAnalyze(const QString &dataName, const QString &normalizationName) const
{
	// Can always analyze two 2D data sources.
	if (sources_.count() == 2)
		return true;

	// The first data source is reserved for the data.
	if (indexOfInputSource(dataName) >= 0 && indexOfInputSource(normalizationName))
		return true;

	return false;
}

AMNumber AMNormalizationAB::value(const AMnDIndex &indexes) const
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

bool AMNormalizationAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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

AMNumber AMNormalizationAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber < 0 && axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return sources_.at(0)->axisValue(axisNumber, index);
}

bool AMNormalizationAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber < 0 && axisNumber >= rank())
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return sources_.at(0)->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AMNormalizationAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	cacheUpdateRequired_ = true;
	emitValuesChanged(start, end);
}

void AMNormalizationAB::onInputSourceSizeChanged()
{
	for (int i = 0, size = rank(); i < size; i++)
		axes_[i].size = sources_.at(0)->size(i);

	cacheUpdateRequired_ = true;
	cachedData_ = QVector<double>(size().product());
	emitSizeChanged();
}

void AMNormalizationAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AMNormalizationAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
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
	/*
	else {

		sources_ = dataSources;
		axes_.clear();

		for (int i = 0, size = sources_.at(0)->rank(); i < size; i++)
			axes_.append(sources_.at(0)->axisInfoAt(i));

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("Sum of spectra from %1 maps").arg(sources_.size()));

		for (int i = 0; i < sources_.size(); i++){

			connect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			connect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			connect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}
	*/
	else {
		sources_ = dataSources;
		setInputSources();
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged(AMnDIndex(0), size()-1);
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMNormalizationAB::reviewState()
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

void AMNormalizationAB::setInputSources()
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (normalizer_){

		disconnect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		normalizer_ = 0;
	}

	int dataIndex = indexOfInputSource(dataName_);
	int normalizationIndex = indexOfInputSource(normalizationName_);

	if (dataIndex >= 0 && normalizationIndex >= 0){

		data_ = inputDataSourceAt(dataIndex);
		normalizer_ = inputDataSourceAt(normalizationIndex);
		canAnalyze_ = true;

		axes_.clear();
		for (int i = 0, size = sources_.at(0)->rank(); i < size; i++)
			axes_ << sources_.at(0)->axisInfoAt(i);

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("Normalized %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		data_ = 0;
		normalizer_ = 0;
		canAnalyze_ = false;

		setDescription("Normalized 2D Data Source");
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged(AMnDIndex(0), size()-1);
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMNormalizationAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(rank(), AMnDIndex::DoInit, 0);
	AMnDIndex end = size()-1;
	int totalSize = start.totalPointsTo(end);

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> normalizer = QVector<double>(totalSize);

	data_->values(start, end, data.data());
	normalizer_->values(start, end, normalizer.data());

	for (int i = 0; i < totalSize; i++){

	    if (normalizer.at(i) == 0)
		cachedData_[i] = 0;

	    else if (normalizer.at(i) < 0 || data.at(i) == -1)
		cachedData_[i] = -1;

	    else
		cachedData_[i] = data.at(i)/normalizer.at(i);
	}

	cachedDataRange_ = AMUtility::rangeFinder(cachedData_, -1);
	cacheUpdateRequired_ = false;
}

