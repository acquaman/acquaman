#include "AMRegionOfInterestAB.h"

 AMRegionOfInterestAB::~AMRegionOfInterestAB(){}
AMRegionOfInterestAB::AMRegionOfInterestAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectrum_ = 0;
	binningRange_ = AMRange();

	setState(AMDataSource::InvalidFlag);
}

bool AMRegionOfInterestAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable; the null input.

	// otherwise there are three data sources
	if (dataSources.size() == 1)
		return dataSources.first()->rank() == 1;

	return false;
}

void AMRegionOfInterestAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(spectrum_ != 0) {

		disconnect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		spectrum_ = 0;
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		spectrum_ = dataSources.first();
		sources_ = dataSources;

		setDescription(name());

		connect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

AMNumber AMRegionOfInterestAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != rank())
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexes.at(i) >= axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (!binningRange_.isValid())
		return AMNumber(AMNumber::InvalidError);

	QVector<double> data = QVector<double>(int(binningRange_.maximum()-binningRange_.minimum()+1));

	// Need to turn the range into index positions.
	int minimum = binningRange_.minimum()/double(spectrum_->axisInfoAt(0).increment);
	int maximum = binningRange_.maximum()/double(spectrum_->axisInfoAt(0).increment);

	if (spectrum_->values(minimum, maximum, data.data())){

		double value = 0;

		for (int i = 0, size = data.size(); i < size; i++)
			value += data.at(i);

		return value;
	}

	return AMNumber(AMNumber::InvalidError);
}

bool AMRegionOfInterestAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != rank() || indexEnd.rank() != indexStart.rank())
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexEnd.at(i) >= axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;
#endif

	if (!binningRange_.isValid())
		return false;

	QVector<double> data = QVector<double>(int(binningRange_.maximum()-binningRange_.minimum()+1));

	// Need to turn the range into index positions.
	int minimum = binningRange_.minimum()/double(spectrum_->axisInfoAt(0).increment);
	int maximum = binningRange_.maximum()/double(spectrum_->axisInfoAt(0).increment);

	if (spectrum_->values(minimum, maximum, data.data())){

		double value = 0;

		for (int i = 0, size = data.size(); i < size; i++)
			value += data.at(i);

		*outputValues = value;

		return true;
	}

	return false;
}

void AMRegionOfInterestAB::setBinningRange(const AMRange &newRange)
{
	binningRange_ = newRange;
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeLowerBound(double lowerBound)
{
	binningRange_.setMinimum(lowerBound);
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeUpperBound(double upperBound)
{
	binningRange_.setMaximum(upperBound);
	emitValuesChanged();
}

AMNumber AMRegionOfInterestAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber < 0 && axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (index < 0 || index >= spectrum_->size(rank()))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	return AMNumber(AMNumber::Null);
}

void AMRegionOfInterestAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AMRegionOfInterestAB::onInputSourceSizeChanged()
{
	emitSizeChanged();
}

void AMRegionOfInterestAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AMRegionOfInterestAB::reviewState()
{
	if (spectrum_ == 0 || !spectrum_->isValid()){

		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

bool AMRegionOfInterestAB::loadFromDb(AMDatabase *db, int id)
{
	bool success = AMDbObject::loadFromDb(db, id);
	if(success)
		AMDataSource::name_ = AMDbObject::name(); /// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
	return success;
}
