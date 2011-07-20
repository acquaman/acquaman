#include "AMDeadTimeAB.h"

AMDeadTimeAB::AMDeadTimeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectra_ = 0;
	icr_ = 0;
	ocr_ = 0;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}
#include <QDebug>
// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source
- the rank() of that input source must be 1 (one-dimensional)
*/
bool AMDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable; the null input.

	// otherwise there are three data sources, one with rank 1 and two with rank 0.
	if (dataSources.count() == 3 && (dataSources.at(0)->rank()+dataSources.at(1)->rank()+dataSources.at(2)->rank() == 1))
		return true;

	return false;
}

// Set the data source inputs.
void AMDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(spectra_ != 0 && icr_ != 0 && ocr_ != 0) {

		disconnect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		disconnect(icr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(icr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(icr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		disconnect(ocr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(ocr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(ocr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		spectra_ = dataSources.first();
		icr_ = dataSources.at(1);
		ocr_ = dataSources.at(2);

		sources_ = dataSources;

		axes_[0] = spectra_->axisInfoAt(0);

		setDescription(QString("Dead time correction of %1").arg(spectra_->description()));

		connect(spectra_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectra_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectra_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(icr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(icr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(icr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(ocr_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(ocr_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(ocr_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

AMNumber AMDeadTimeAB::value(const AMnDIndex &indexes, bool doBoundsChecking) const
{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (doBoundsChecking && indexes.i() >= spectra_->size(0))
		return AMNumber(AMNumber::OutOfBoundsError);

	if ((int)spectra_->value(indexes.i()) == 0)
		return 0;
	else
		return double(icr_->value(AMnDIndex()))/double(ocr_->value(AMnDIndex()))*(int)spectra_->value(indexes.i());
}

AMNumber AMDeadTimeAB::axisValue(int axisNumber, int index, bool doBoundsChecking) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (doBoundsChecking && index >= spectra_->size(0))
		return AMNumber(AMNumber::OutOfBoundsError);

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
	if (spectra_ == 0 || icr_ == 0 || ocr_ == 0 || !spectra_->isValid() || !icr_->isValid() || !ocr_->isValid()){

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
