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


#include "AM3DDeadTimeAB.h"

 AM3DDeadTimeAB::~AM3DDeadTimeAB(){}
AM3DDeadTimeAB::AM3DDeadTimeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectra_ = 0;
	icr_ = 0;
	ocr_ = 0;

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a data source of rank() = 3.
   - there must be two data sources of rank() = 2.
*/
bool AM3DDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise there are three data sources, one with rank 3 and two with rank 2.
	if (dataSources.count() == 3
			&& dataSources.at(0)->rank() == 3
			&& dataSources.at(1)->rank() == 2
			&& dataSources.at(2)->rank() == 2)
		return true;

	return false;
}

AMNumber AM3DDeadTimeAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != 3)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (indexes.i() >= spectra_->size(0)
			|| indexes.j() >= spectra_->size(1)
			|| indexes.k() >= spectra_->size(2))
		return AMNumber(AMNumber::OutOfBoundsError);

	if ((int)spectra_->value(indexes) == 0 || double(ocr_->value(AMnDIndex(indexes.i(), indexes.j()))) == 0)
		return 0;
	else
		return double(icr_->value(AMnDIndex(indexes.i(), indexes.j())))/double(ocr_->value(AMnDIndex(indexes.i(), indexes.j())))*(int)spectra_->value(indexes);
}

bool AM3DDeadTimeAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 3 || indexEnd.rank() != 3)
		return false;

	if(!isValid())
		return false;

	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j()
			|| (unsigned)indexEnd.k() >= (unsigned)axes_.at(2).size || (unsigned)indexStart.k() > (unsigned)indexEnd.k())
		return false;

	int totalSize = indexStart.totalPointsTo(indexEnd);
	AMnDIndex start2D = AMnDIndex(indexStart.i(), indexStart.j());
	AMnDIndex end2D = AMnDIndex(indexEnd.i(), indexEnd.j());
	int icrOcrTotalSize = start2D.totalPointsTo(end2D);

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> icr = QVector<double>(icrOcrTotalSize);
	QVector<double> ocr = QVector<double>(icrOcrTotalSize);
	spectra_->values(indexStart, indexEnd, data.data());
	icr_->values(start2D, end2D, icr.data());
	ocr_->values(start2D, end2D, ocr.data());

	for (int i = 0, iSize = indexEnd.i()-indexStart.i()+1; i < iSize; i++){

		for (int j = 0, jSize = indexEnd.j()-indexStart.j()+1; j < jSize; j++){

			// If ocr is equal to 0 then that will cause division by zero.  Since these are both count rates, they should both be greater than zero.
			if (icr.at(i*jSize+j) <= 0 || ocr.at(i*jSize+j) <= 0){

				for (int k = 0, kSize = indexEnd.k()-indexStart.k()+1; k < kSize; k++)
					outputValues[i*jSize*kSize+j*kSize+k] = 0;
			}

			else {

				double factor = icr.at(i*jSize+j)/ocr.at(i*jSize+j);

				for (int k = 0, kSize = indexEnd.k()-indexStart.k()+1; k < kSize; k++)
					outputValues[i*jSize*kSize+j*kSize+k] = data.at(i*jSize*kSize+j*kSize+k)*factor;
			}
		}
	}

	return true;
}

AMNumber AM3DDeadTimeAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	return spectra_->axisValue(axisNumber, index);
}

bool AM3DDeadTimeAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return spectra_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

// Connected to be called when the values of the input data source change
void AM3DDeadTimeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	if (start.rank() == axes_.size() && end.rank() == axes_.size())
		emitValuesChanged(start, end);

	else
		emitValuesChanged();
}

// Connected to be called when the size of the input source changes
void AM3DDeadTimeAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != spectra_->size(0)){

		axes_[0].size = spectra_->size(0);
		emitSizeChanged(0);
	}

	if(axes_.at(1).size != spectra_->size(1)){

		axes_[1].size = spectra_->size(1);
		emitSizeChanged(1);
	}

	if(axes_.at(2).size != spectra_->size(2)){

		axes_[2].size = spectra_->size(2);
		emitSizeChanged(2);
	}
}

// Connected to be called when the state() flags of any input source change
void AM3DDeadTimeAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

// Set the data source inputs.
void AM3DDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
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

		spectra_ = 0;
		icr_ = 0;
		ocr_ = 0;
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		spectra_ = dataSources.first();
		icr_ = dataSources.at(1);
		ocr_ = dataSources.at(2);

		sources_ = dataSources;

		axes_[0] = spectra_->axisInfoAt(0);
		axes_[1] = spectra_->axisInfoAt(1);
		axes_[2] = spectra_->axisInfoAt(2);

		setDescription(QString("Corrected %1").arg(spectra_->description()));

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
	emitSizeChanged(1);
	emitSizeChanged(2);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitAxisInfoChanged(1);
	emitAxisInfoChanged(2);
	emitInfoChanged();
}

void AM3DDeadTimeAB::reviewState()
{
	if (spectra_ == 0 || icr_ == 0 || ocr_ == 0 || !spectra_->isValid() || !icr_->isValid() || !ocr_->isValid()){

		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}
