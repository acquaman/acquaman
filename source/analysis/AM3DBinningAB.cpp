#include "AM3DBinningAB.h"

AM3DBinningAB::AM3DBinningAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	sumAxis_ = 0;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	inputSource_ = 0;
	cacheCompletelyInvalid_ = true;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

AM3DBinningAB::AM3DBinningAB(AMDatabase *db, int id)
	: AMStandardAnalysisBlock("tempName")
{
	sumAxis_ = 0;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;

	inputSource_ = 0;
	cacheCompletelyInvalid_ = true;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);

	loadFromDb(db, id);
		// will restore sumAxis, sumRangeMin, and sumRangeMax. We'll remain invalid until we get connected.

	AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 3D data sources
	- the rank() of that input source must be 3 (two-dimensional)
	*/
bool AM3DBinningAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

	if(dataSources.isEmpty())
		return true;	// always acceptable; the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 3)
		return true;

	// Or we need a list of 2D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 3)
				return false;

		return true;
	}

	return false;
}


/// Set the data source inputs.
void AM3DBinningAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		inputSource_ = 0;
	}

	if(dataSources.isEmpty()) {
		inputSource_ = 0;
		sources_.clear();
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else if (dataSources.count() == 1) {

		inputSource_ = dataSources.at(0);
		sources_ = dataSources;
		canAnalyze_ = true;

		switch (sumAxis_){

		case 0:
			axes_[0] = inputSource_->axisInfoAt(1);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			break;
		}

		// Have to call into AMDataStore directly to avoid setModified(true)
		AMDataSource::setDescription(QString("%1 Summed (over %2)")
						 .arg(inputSource_->name())
						 .arg(inputSource_->axisInfoAt(sumAxis_).name));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	else {

		sources_ = dataSources;
		setInputSource();
	}

	invalidateCache();
	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM3DBinningAB::setAnalyzedName(const QString &name)
{
	if(analyzedName_ == name)
		return;
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();

	invalidateCache();
	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM3DBinningAB::setInputSource()
{
	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		inputSource_ = 0;
	}

	int index = indexOfInputSource(analyzedName_);

	if (index >= 0){

		inputSource_ = inputDataSourceAt(index);
		canAnalyze_ = true;

		switch (sumAxis_){

		case 0:
			axes_[0] = inputSource_->axisInfoAt(1);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			break;
		}

		setDescription(QString("%1 Summed (over %2)")
					   .arg(inputSource_->name())
					   .arg(inputSource_->axisInfoAt(sumAxis_).name));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		inputSource_ = 0;
		canAnalyze_ = false;
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Sum");
	}
}

bool AM3DBinningAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 3D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM3DBinningAB::value(const AMnDIndex& indexes) const {
	if(indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size
			|| (unsigned)indexes.j() >= (unsigned)axes_.at(1).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	int index = indexes.i()+indexes.j()*axes_.at(0).size;
	double rv = cachedValues_.at(index);
	// if we haven't calculated this sum yet, the cached value will be invalid. Sum and store.
	if(rv == AM3DMAGICNUMBER) {

		double newVal = 0.0;

		switch (sumAxis_){

		case 0:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(i, indexes.i(), indexes.j()));

			break;

		case 1:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), i, indexes.j()));

			break;

		case 2:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), indexes.j(), i));

			break;
		}

		cachedValues_[index] = newVal;
		cacheCompletelyInvalid_ = false;
		return newVal;
	}
	// otherwise return the value we have.
	else
		return rv;
}
#include <QDebug>
bool AM3DBinningAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 2 || indexEnd.rank() != 2)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j())
		return false;
#endif


	AMnDIndex nDImageSize = inputSource_->size();
	int imageSize = AMnDIndex(0, 0, 0).totalPointsTo(nDImageSize);

	QVector<double> data = QVector<double>(imageSize);
	inputSource_->values(AMnDIndex(0, 0, 0), nDImageSize, data.data());

	QTime timer;
	timer.start();

	switch (sumAxis_){

	case 0:{

		for (int j = indexStart.i(), jSize = indexEnd.i(); j < jSize; j++){

			for (int k = indexStart.j(), kSize = indexEnd.j(); k < kSize; k++){

				double sum = 0;

				for (int i = sumRangeMin_; i < sumRangeMax_; i++)
					sum += data[i*axes_.at(0).size + j*axes_.at(1).size + k];

				*(outputValues++) = sum;
			}
		}

		break;
	}

	case 1: {

		for (int i = indexStart.i(), iSize = indexEnd.i(); i < iSize; i++){

			for (int k = indexStart.j(), kSize = indexEnd.j(); k < kSize; k++){

				double sum = 0;

				for (int j = sumRangeMin_; j < sumRangeMax_; j++)
					sum += data[i*axes_.at(0).size + j*axes_.at(1).size + k];

				*(outputValues++) = sum;
			}
		}

		break;
	}

	case 2: {

		for (int i = indexStart.i(), iSize = indexEnd.i(); i < iSize; i++){

			for (int j = indexStart.j(), jSize = indexEnd.j(); j < jSize; j++){

				double sum = 0;

				for (int k = sumRangeMin_; k < sumRangeMax_; k++)
					sum += data[i*axes_.at(0).size + j*axes_.at(1).size + k];

				*(outputValues++) = sum;
			}
		}

		break;
	}
	}

	qDebug() << QString("Time to compute sum: %1 ms").arg(timer.restart());
	outputValues -= indexStart.totalPointsTo(indexEnd);

	for (int i = 0, offset = indexStart.product(), count = indexStart.totalPointsTo(indexEnd); i < count; i++)
		cachedValues_[i+offset] = outputValues[i];

	qDebug() << QString("Time to set the cached values: %1").arg(timer.elapsed());
	cacheCompletelyInvalid_ = false;

	return true;
}

AMNumber AM3DBinningAB::axisValue(int axisNumber, int index) const {

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 || axisNumber != 1)
		return AMNumber(AMNumber::DimensionError);

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber == 0 ? 1 : 2;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? 0 : 2;
		break;

	case 2:
		actualAxis = axisNumber == 0 ? 0 : 1;
		break;
	}

	return inputSource_->axisValue(actualAxis, index);
}

// Connected to be called when the values of the input data source change
void AM3DBinningAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {

	if(start.isValid() && end.isValid()) {

		int offset = start.product();
		int totalPoints = start.totalPointsTo(end);
		for(int i = offset, count = totalPoints + offset; i < count; i++)
			cachedValues_[i] = AM3DMAGICNUMBER;	// invalidate the changed region
		emitValuesChanged(start, end);
	}
	else {
		invalidateCache();
		emitValuesChanged();
	}

	/// \todo start an idle-time computation of the cached values?
}

/// Connected to be called when the size of the input source changes
void AM3DBinningAB::onInputSourceSizeChanged()
{
	switch (sumAxis_){

	case 0:

		if (axes_.at(0).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(2).size){

			axes_[0].size = inputSource_->axisInfoAt(1).size;
			axes_[1].size = inputSource_->axisInfoAt(2).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size);
			emitSizeChanged();
		}

		break;

	case 1:

		if (axes_.at(0).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(2).size){

			axes_[0].size = inputSource_->axisInfoAt(0).size;
			axes_[1].size = inputSource_->axisInfoAt(2).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size);
			emitSizeChanged();
		}

		break;

	case 2:

		if (axes_.at(0).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(2).size){

			axes_[0].size = inputSource_->axisInfoAt(0).size;
			axes_[1].size = inputSource_->axisInfoAt(1).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size);
			emitSizeChanged();
		}

		break;
	}
}

/// Connected to be called when the state() flags of any input source change
void AM3DBinningAB::onInputSourceStateChanged() {

	reviewState();

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
}

#include "analysis/AM3DBinningABEditor.h"

QWidget* AM3DBinningAB::createEditorWidget()
{
	return new AM3DBinningABEditor(this);
}

