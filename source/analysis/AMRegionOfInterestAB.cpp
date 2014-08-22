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
	}

	// we know that this will only be called with valid input source
	else {

		spectrum_ = dataSources.first();
		sources_ = dataSources;

		setDescription(name());
		axes_.clear();

		for (int i = 0, size = spectrum_->rank()-1; i < size; i++)
			axes_.append(spectrum_->axisInfoAt(i));

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

	// Need to turn the range into index positions.
	int minimum = int(binningRange_.minimum()/double(spectrum_->axisInfoAt(spectrum_->rank()-1).increment));
	int maximum = int(binningRange_.maximum()/double(spectrum_->axisInfoAt(spectrum_->rank()-1).increment));

	AMnDIndex start = AMnDIndex(indexes.rank()+1, AMnDIndex::DoNotInit);
	AMnDIndex end = AMnDIndex(indexes.rank()+1, AMnDIndex::DoNotInit);

	for (int i = 0, size = indexes.rank(); i < size; i++){

		start[i] = indexes.at(i);
		end[i] = indexes.at(i);
	}

	start[indexes.rank()] = minimum;
	end[indexes.rank()] = maximum;

	QVector<double> data = QVector<double>(maximum - minimum + 1);

	if (spectrum_->values(start, end, data.data())){

		// If negative, then there isn't real data yet.
		if (data.at(0) < 0)
			return -1.0;

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

	// Need to turn the range into index positions.
	int minimum = int(binningRange_.minimum()/double(spectrum_->axisInfoAt(spectrum_->rank()-1).increment));
	int maximum = int(binningRange_.maximum()/double(spectrum_->axisInfoAt(spectrum_->rank()-1).increment));
	int axisLength = maximum - minimum + 1;

	AMnDIndex start = AMnDIndex(indexStart.rank()+1, AMnDIndex::DoNotInit);
	AMnDIndex end = AMnDIndex(indexEnd.rank()+1, AMnDIndex::DoNotInit);

	for (int i = 0, size = indexStart.rank(); i < size; i++){

		start[i] = indexStart.at(i);
		end[i] = indexEnd.at(i);
	}

	start[indexStart.rank()] = minimum;
	end[indexEnd.rank()] = maximum;

	QVector<double> data = QVector<double>(start.totalPointsTo(end));

	if (spectrum_->values(start, end, data.data())){

		switch(rank()){

		case 0:{

			double value = 0;

			for (int i = 0, size = data.size(); i < size; i++)
				value += data.at(i);

			*outputValues = (value < 0 ? -1 : value);

			break;
		}

		case 1:{

			for (int i = 0, size = indexStart.totalPointsTo(indexEnd); i < size; i++){

				double value = 0;

				for (int j = 0, jSize = maximum - minimum + 1; j < jSize; j++)
					value += data.at(i*axisLength+j);

				outputValues[i] = (value < 0 ? -1 : value);
			}

			break;
		}

		case 2:{

			for (int i = 0, iSize = end.i()-start.i()+1; i < iSize; i++){

				for (int j = 0, jSize = end.j()-start.j()+1; j < jSize; j++){

					double value = 0;

					for (int k = 0, kSize = maximum - minimum + 1; k < kSize; k++)
						value += data.at(i*jSize*axisLength+j*axisLength+k);

					outputValues[i*jSize+j] = (value < 0 ? -1 : value);
				}
			}

			break;
		}
		}

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

	if (rank() == 0)
		return AMNumber(AMNumber::Null);

	return spectrum_->axisValue(axisNumber, index);
}

void AMRegionOfInterestAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AMRegionOfInterestAB::onInputSourceSizeChanged()
{
	for (int i = 0, size = axes_.size(); i < size; i++){

		if (axes_.at(i).size != spectrum_->size(i)){

			axes_[i].size = spectrum_->size(i);
			emitSizeChanged(i);
		}
	}
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
