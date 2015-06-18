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


#include "AMRegionOfInterestAB.h"

AMRegionOfInterestAB::AMRegionOfInterestAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectrum_ = 0;
	binningRange_ = AMRange();

	dataUpdateRequired_ = false;
	lowerDirtyIndex_ = AMnDIndex();
	upperDirtyIndex_ = AMnDIndex();
	cachedOutput_ = QVector<double>();

	setState(AMDataSource::InvalidFlag);
}

AMRegionOfInterestAB::~AMRegionOfInterestAB(){}

bool AMRegionOfInterestAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise there is one data source
	if (dataSources.size() == 1)
		return dataSources.first()->rank() >= 1;

	return false;
}

AMNumber AMRegionOfInterestAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != rank())
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, axisSize = axes_.size(); i < axisSize; i++)
		if (indexes.at(i) >= axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (!binningRange_.isValid())
		return AMNumber(AMNumber::InvalidError);

	if (dataUpdateRequired_)
		computeCachedValues();

	AMNumber returnValue;

	switch(rank()){

	case 0:

		returnValue = cachedOutput_.at(0);
		break;

	case 1:

		returnValue = cachedOutput_.at(indexes.i());
		break;

	case 2:

		returnValue = cachedOutput_.at(indexes.i()*size(1)+indexes.j());
		break;

	case 3:

		returnValue = cachedOutput_.at(indexes.i()*size(1)*size(2)+indexes.j()*size(2)+indexes.k());
		break;
	}

	return returnValue;
}

bool AMRegionOfInterestAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != rank() || indexEnd.rank() != indexStart.rank())
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, axisSize = axes_.size(); i < axisSize; i++)
		if (indexEnd.at(i) >= axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;
#endif

	if (!binningRange_.isValid())
		return false;

	if (dataUpdateRequired_)
		computeCachedValues();

	switch(rank()){

	case 0:

		*outputValues = cachedOutput_.at(0);

		break;

	case 1:{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		int xOffset = indexStart.i();

		memcpy(outputValues, cachedOutput_.constData()+xOffset, totalSize*sizeof(double));

		break;
	}

	case 2:{

		int totalSize = indexStart.totalPointsTo(indexEnd);

		if (totalSize == size().product())
			memcpy(outputValues, cachedOutput_.constData(), totalSize*sizeof(double));

		else {

			int yLength = indexStart.distanceTo(1, indexEnd);
			int xOffset = indexStart.i();
			int yOffset = indexStart.j();
			int ySize = size(1);
			const double *cachedOutputDoublePointer = cachedOutput_.constData();

			for (int i = 0, iSize = indexStart.distanceTo(0, indexEnd); i < iSize; i++)
				memcpy(outputValues+yLength*i, cachedOutputDoublePointer+((i+xOffset)*ySize+yOffset), yLength*sizeof(double));
		}

		break;
	}

	case 3:{

		int yLength = indexStart.distanceTo(1, indexEnd);
		int zLength = indexStart.distanceTo(2, indexEnd);
		int xOffset = indexStart.i();
		int yOffset = indexStart.j();
		int zOffset = indexStart.k();
		int ySize = size(1);
		int zSize = size(2);
		const double *cachedOutputDoublePointer = cachedOutput_.constData();

		for (int i = 0, iSize = indexStart.distanceTo(0, indexEnd); i < iSize; i++){

			for (int j = 0, jSize = indexStart.distanceTo(0, indexEnd); j < jSize; j++)
				memcpy(outputValues+(i*yLength*zLength+j*zLength), cachedOutputDoublePointer+((i+xOffset)*ySize*zSize+(j+yOffset)*zSize+zOffset), zLength*sizeof(double));
		}

		break;
	}
	}

	return true;
}

void AMRegionOfInterestAB::setBinningRange(const AMRange &newRange)
{
	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;

	binningRange_ = newRange;
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeLowerBound(double lowerBound)
{
	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;

	binningRange_.setMinimum(lowerBound);
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeUpperBound(double upperBound)
{
	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;

	binningRange_.setMaximum(upperBound);
	emitValuesChanged();
}

AMNumber AMRegionOfInterestAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber < 0 || axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (index < 0 || index >= spectrum_->size(rank()))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (rank() == 0)
		return AMNumber(AMNumber::Null);

	return spectrum_->axisValue(axisNumber, index);
}

bool AMRegionOfInterestAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber < 0 || axisNumber >= rank())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (startIndex < 0 || startIndex >= spectrum_->size(rank()) || endIndex < 0 || endIndex >= spectrum_->size(rank()))
		return false;
#endif

	if (rank() == 0)
		return false;

	return spectrum_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AMRegionOfInterestAB::computeCachedValues() const
{
	// Need to turn the range into index positions.
	AMAxisInfo axisInfoOfInterest = spectrum_->axisInfoAt(spectrum_->rank()-1);

	int minimum = int((binningRange_.minimum() - double(axisInfoOfInterest.start))/double(axisInfoOfInterest.increment));
	int maximum = int((binningRange_.maximum() - double(axisInfoOfInterest.start))/double(axisInfoOfInterest.increment));
	int axisLength = maximum - minimum + 1;

	AMnDIndex start = AMnDIndex(lowerDirtyIndex_.rank()+1, AMnDIndex::DoNotInit);
	AMnDIndex end = AMnDIndex(upperDirtyIndex_.rank()+1, AMnDIndex::DoNotInit);

	for (int i = 0, dirtyRank = lowerDirtyIndex_.rank(); i < dirtyRank; i++){

		start[i] = lowerDirtyIndex_.at(i);
		end[i] = upperDirtyIndex_.at(i);
	}

	start[lowerDirtyIndex_.rank()] = minimum;
	end[upperDirtyIndex_.rank()] = maximum;

	QVector<double> data = QVector<double>(start.totalPointsTo(end));

	if (spectrum_->values(start, end, data.data())){

		switch(rank()){

		case 0:{

			double value = 0;

			for (int i = 0, size = data.size(); i < size; i++)
				value += data.at(i);

			cachedOutput_[0] = (value < 0 ? -1 : value);

			break;
		}

		case 1:{

			int xOffset = lowerDirtyIndex_.i();

			for (int i = 0, size = lowerDirtyIndex_.distanceTo(0, upperDirtyIndex_); i < size; i++){

				double value = 0;

				for (int j = 0, jSize = maximum - minimum + 1; j < jSize; j++)
					value += data.at(i*axisLength+j);

				cachedOutput_[i+xOffset] = (value < 0 ? -1 : value);
			}

			break;
		}

		case 2:{

			int xOffset = lowerDirtyIndex_.i();
			int yOffset = lowerDirtyIndex_.j();
			int ySize = size(1);

			for (int i = 0, iSize = lowerDirtyIndex_.distanceTo(0, upperDirtyIndex_); i < iSize; i++){

				for (int j = 0, jSize = lowerDirtyIndex_.distanceTo(1, upperDirtyIndex_); j < jSize; j++){

					double value = 0;

					for (int k = 0, kSize = maximum - minimum + 1; k < kSize; k++)
						value += data.at(i*jSize*axisLength+j*axisLength+k);

					cachedOutput_[(i+xOffset)*ySize+(j+yOffset)] = (value < 0 ? -1 : value);
				}
			}

			break;
		}

		case 3:{

			int xOffset = lowerDirtyIndex_.i();
			int yOffset = lowerDirtyIndex_.j();
			int zOffset = lowerDirtyIndex_.k();
			int ySize = size(1);
			int zSize = size(2);

			for (int i = 0, iSize = lowerDirtyIndex_.distanceTo(0, upperDirtyIndex_); i < iSize; i++){

				for (int j = 0, jSize = lowerDirtyIndex_.distanceTo(1, upperDirtyIndex_); j < jSize; j++){

					for (int k = 0, kSize = lowerDirtyIndex_.distanceTo(2, upperDirtyIndex_); k < kSize; k++){

						double value = 0;

						for (int l = 0, lSize = maximum - minimum + 1; l < lSize; l++)
							value += data.at(i*jSize*kSize*axisLength+j*kSize*axisLength+k*axisLength+l);

						cachedOutput_[(i+xOffset)*ySize*zSize+(j+yOffset)*zSize+(k+zOffset)] = (value < 0 ? -1 : value);
					}
				}
			}

			break;
		}
		}

		dataUpdateRequired_ = false;
		lowerDirtyIndex_ = AMnDIndex();
		upperDirtyIndex_ = AMnDIndex();
	}
}

void AMRegionOfInterestAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	if (!dataUpdateRequired_)
		dataUpdateRequired_ = true;

	lowerDirtyIndex_ = lowerDirtyIndex_.globalMinimum(start);
	upperDirtyIndex_ = upperDirtyIndex_.globalMaximum(end);

	emitValuesChanged(start, end);
}

void AMRegionOfInterestAB::onInputSourceSizeChanged()
{
	for (int i = 0, numberOfAxes = axes_.size(); i < numberOfAxes; i++){

		if (axes_.at(i).size != spectrum_->size(i)){

			axes_[i].size = spectrum_->size(i);
			emitSizeChanged(i);
		}
	}

	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;
	cachedOutput_ = QVector<double>(size().product());
}

void AMRegionOfInterestAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
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
		axes_.clear();

		dataUpdateRequired_ = false;
		lowerDirtyIndex_ = AMnDIndex();
		upperDirtyIndex_ = AMnDIndex();
		cachedOutput_ = QVector<double>();
	}

	// we know that this will only be called with valid input source
	else {

		spectrum_ = dataSources.first();
		sources_ = dataSources;

		setDescription(name());
		axes_.clear();

		for (int i = 0, spectrumRank = spectrum_->rank()-1; i < spectrumRank; i++)
			axes_.append(spectrum_->axisInfoAt(i));

		connect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		dataUpdateRequired_ = true;
		lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
		upperDirtyIndex_ = size()-1;
		cachedOutput_ = QVector<double>(size().product());
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
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
