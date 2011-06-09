#include "AMDeadTimeAB.h"

AMDeadTimeAB::AMDeadTimeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectra_ = 0;
	deadTime_ = 0;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source
- the rank() of that input source must be 1 (one-dimensional)
*/
bool AMDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable; the null input.

	// otherwise there is two data sources, one with rank 1 and one with rank 0.
	if (dataSources.count() == 2
		|| (dataSources.at(0)->rank() == 0 && dataSources.at(1)->rank() == 1)
		|| (dataSources.at(0)->rank() == 1 && dataSources.at(1)->rank() == 0))
		return true;

	return false;
}

// Set the data source inputs.
void AMDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(spectra_ != 0 && deadTime_ != 0) {

		disconnect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		disconnect(deadTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(deadTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(deadTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		// If the spectra data source comes first...
		if (dataSources.first()->rank() == 1){

			spectra_ = dataSources.first();
			deadTime_ = dataSources.at(1);
		}
		// Otherwise...
		else {

			spectra_ = dataSources.at(1);
			deadTime_ = dataSources.first();
		}

		sources_ = dataSources;

		axes_[0] = spectra_->axisInfoAt(0);

		setDescription(QString("Dead time correction of %1")
		.arg(spectra_->name()));

		connect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(deadTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(deadTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(deadTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

AMNumber AMDeadTimeAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	return (1/(1-(double)deadTime_->value(AMnDIndex())/100))*(int)spectra_->value(indexes.i());
}

AMNumber AMDeadTimeAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return spectra_->axisValue(axisNumber, index);
}

// Connected to be called when the values of the input data source change
void AMDeadTimeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

// Connected to be called when the size of the input source changes
void AMDeadTimeAB::onInputSourceSizeChanged()
{
	axes_[0] = spectra_->axisInfoAt(0);
	emitSizeChanged();
}

// Connected to be called when the state() flags of any input source change
void AMDeadTimeAB::onInputSourceStateChanged()
{
	reviewState();

	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
}

void AMDeadTimeAB::reviewState()
{
	if (spectra_ == 0 || deadTime_ == 0 || !spectra_->isValid() || !deadTime_->isValid()){

		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

bool AMDeadTimeAB::loadFromDb(AMDatabase *db, int id)
{
	bool success = AMDbObject::loadFromDb(db, id);
	if(success)
		AMDataSource::name_ = AMDbObject::name(); /// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
	return success;
}
