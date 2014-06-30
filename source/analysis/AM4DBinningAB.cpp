/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AM4DBinningAB.h"

#include "AM4DBinningAB.h"

 AM4DBinningAB::~AM4DBinningAB(){}
AM4DBinningAB::AM4DBinningAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	sumAxis_ = 3;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	inputSource_ = 0;
	cacheCompletelyInvalid_ = true;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

AM4DBinningAB::AM4DBinningAB(AMDatabase *db, int id)
	: AMStandardAnalysisBlock("tempName")
{
	sumAxis_ = 3;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;

	inputSource_ = 0;
	cacheCompletelyInvalid_ = true;
	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);

	loadFromDb(db, id);
		// will restore sumAxis, sumRangeMin, and sumRangeMax. We'll remain invalid until we get connected.

	AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 4D data sources
	- the rank() of that input source must be 4 (four-dimensional)
	*/
bool AM4DBinningAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

	if(dataSources.isEmpty())
		return true;	// always acceptable; the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 4)
		return true;

	// Or we need a list of 2D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 4)
				return false;

		return true;
	}

	return false;
}


// Set the data source inputs.
void AM4DBinningAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

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
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");
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
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 3:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(2);
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

void AM4DBinningAB::setAnalyzedName(const QString &name)
{
	if(analyzedName_ == name)
		return;
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

void AM4DBinningAB::setInputSource()
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
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 3:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(2);
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
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Sum");
	}

	invalidateCache();
	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

bool AM4DBinningAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 3D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM4DBinningAB::value(const AMnDIndex& indexes) const {
	if(indexes.rank() != 3)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size
			|| (unsigned)indexes.j() >= (unsigned)axes_.at(1).size
			|| (unsigned)indexes.k() >= (unsigned)axes_.at(2).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	int index = indexes.i()+indexes.j()*axes_.at(0).size+indexes.k()*axes_.at(1).size*axes_.at(2).size;
	double rv = cachedValues_.at(index);
	// if we haven't calculated this sum yet, the cached value will be invalid. Sum and store.
	if(rv == AM3DMAGICNUMBER) {

		double newVal = 0.0;

		switch (sumAxis_){

		case 0:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(i, indexes.i(), indexes.j(), indexes.k()));

			break;

		case 1:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), i, indexes.j(), indexes.k()));

			break;

		case 2:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), indexes.j(), i, indexes.k()));

			break;

		case 3:

			for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
				newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), indexes.j(), indexes.k(), i));

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

bool AM4DBinningAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 3 || indexEnd.rank() != 3)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j()
			|| (unsigned)indexEnd.k() >= (unsigned)axes_.at(2).size || (unsigned)indexStart.k() > (unsigned)indexEnd.k())
		return false;
#endif

	int totalPoints = indexStart.totalPointsTo(indexEnd);
	QVector<double> tempOutput;

	switch (sumAxis_){

	case 0:{

		int sumRange = AMnDIndex(0, 0, 0, sumRangeMin_).totalPointsTo(AMnDIndex(0, 0, 0, sumRangeMax_));
		QVector<double> data = QVector<double>(sumRange);

		for (int i = indexStart.i(), iSize = indexEnd.i(); i <= iSize; i++){

			for (int j = indexStart.j(), jSize = indexEnd.j(); j <= jSize; j++){

				for (int k = indexStart.k(), kSize = indexEnd.k(); k <= kSize; k++){

					double sum = 0;
					inputSource_->values(AMnDIndex(sumRangeMin_, i, j, k), AMnDIndex(sumRangeMax_, i, j, k), data.data());

					for (int l = 0; l < sumRange; l++)
						sum += data.at(l);

					tempOutput << sum;
				}
			}
		}

		break;
	}

	case 1: {

		int sumRange = AMnDIndex(0, 0, 0, sumRangeMin_).totalPointsTo(AMnDIndex(0, 0, 0, sumRangeMax_));
		QVector<double> data = QVector<double>(sumRange);

		for (int i = indexStart.i(), iSize = indexEnd.i(); i <= iSize; i++){

			for (int j = indexStart.j(), jSize = indexEnd.j(); j <= jSize; j++){

				for (int k = indexStart.k(), kSize = indexEnd.k(); k <= kSize; k++){

					double sum = 0;
					inputSource_->values(AMnDIndex(i, sumRangeMin_, j, k), AMnDIndex(i, sumRangeMax_, j, k), data.data());

					for (int l = 0; l < sumRange; l++)
						sum += data.at(l);

					tempOutput << sum;
				}
			}
		}

		break;
	}

	case 2: {

		int sumRange = AMnDIndex(0, 0, 0, sumRangeMin_).totalPointsTo(AMnDIndex(0, 0, 0, sumRangeMax_));
		QVector<double> data = QVector<double>(sumRange);

		for (int i = indexStart.i(), iSize = indexEnd.i(); i <= iSize; i++){

			for (int j = indexStart.j(), jSize = indexEnd.j(); j <= jSize; j++){

				for (int k = indexStart.k(), kSize = indexEnd.k(); k <= kSize; k++){

					double sum = 0;
					inputSource_->values(AMnDIndex(i, j, sumRangeMin_, k), AMnDIndex(i, j, sumRangeMax_, k), data.data());

					for (int l = 0; l < sumRange; l++)
						sum += data.at(l);

					tempOutput << sum;
				}
			}
		}

		break;
	}

	case 3: {

		int sumRange = AMnDIndex(0, 0, 0, sumRangeMin_).totalPointsTo(AMnDIndex(0, 0, 0, sumRangeMax_));
		QVector<double> data = QVector<double>(sumRange);

		for (int i = indexStart.i(), iSize = indexEnd.i(); i <= iSize; i++){

			for (int j = indexStart.j(), jSize = indexEnd.j(); j <= jSize; j++){

				for (int k = indexStart.k(), kSize = indexEnd.k(); k <= kSize; k++){

					double sum = 0;
					inputSource_->values(AMnDIndex(i, j, k, sumRangeMin_), AMnDIndex(i, j, k, sumRangeMax_), data.data());

					for (int l = 0; l < sumRange; l++)
						sum += data.at(l);

					tempOutput << sum;
				}
			}
		}

		break;
	}
	}

	memcpy(outputValues, tempOutput.constData(), totalPoints*sizeof(double));

	for (int i = 0, offset = indexStart.product(); i < totalPoints; i++)
		cachedValues_[i+offset] = outputValues[i];

	cacheCompletelyInvalid_ = false;

	return true;
}

AMNumber AM4DBinningAB::axisValue(int axisNumber, int index) const {

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return AMNumber(AMNumber::DimensionError);

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber + 1;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? axisNumber : axisNumber + 1;
		break;

	case 2:
		actualAxis = axisNumber == 3 ? axisNumber + 1 : axisNumber;
		break;

	case 3:
		actualAxis = axisNumber;
		break;
	}

	return inputSource_->axisValue(actualAxis, index);
}

// Connected to be called when the values of the input data source change
void AM4DBinningAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {

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

// Connected to be called when the size of the input source changes
void AM4DBinningAB::onInputSourceSizeChanged()
{
	switch (sumAxis_){

	case 0:

		if (axes_.at(0).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(2).size
				|| axes_.at(2).size != inputSource_->axisInfoAt(3).size){

			axes_[0].size = inputSource_->axisInfoAt(1).size;
			axes_[1].size = inputSource_->axisInfoAt(2).size;
			axes_[2].size = inputSource_->axisInfoAt(3).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size*axes_.at(2).size);
			emitSizeChanged();
		}

		break;

	case 1:

		if (axes_.at(0).size != inputSource_->axisInfoAt(0).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(2).size
				|| axes_.at(2).size != inputSource_->axisInfoAt(3).size){

			axes_[0].size = inputSource_->axisInfoAt(0).size;
			axes_[1].size = inputSource_->axisInfoAt(2).size;
			axes_[2].size = inputSource_->axisInfoAt(3).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size*axes_.at(2).size);
			emitSizeChanged();
		}

		break;

	case 2:

		if (axes_.at(0).size != inputSource_->axisInfoAt(0).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(2).size != inputSource_->axisInfoAt(3).size){

			axes_[0].size = inputSource_->axisInfoAt(0).size;
			axes_[1].size = inputSource_->axisInfoAt(1).size;
			axes_[2].size = inputSource_->axisInfoAt(3).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size*axes_.at(2).size);
			emitSizeChanged();
		}

		break;

	case 3:

		if (axes_.at(0).size != inputSource_->axisInfoAt(0).size
				|| axes_.at(1).size != inputSource_->axisInfoAt(1).size
				|| axes_.at(2).size != inputSource_->axisInfoAt(2).size){

			axes_[0].size = inputSource_->axisInfoAt(0).size;
			axes_[1].size = inputSource_->axisInfoAt(1).size;
			axes_[2].size = inputSource_->axisInfoAt(2).size;
			cachedValues_.resize(axes_.at(0).size*axes_.at(1).size*axes_.at(2).size);
			emitSizeChanged();
		}

		break;
	}
}

// Connected to be called when the state() flags of any input source change
void AM4DBinningAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

#include "analysis/AM4DBinningABEditor.h"

QWidget* AM4DBinningAB::createEditorWidget()
{
	return new AM4DBinningABEditor(this);
}

