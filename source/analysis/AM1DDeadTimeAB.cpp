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


#include "AM1DDeadTimeAB.h"

 AM1DDeadTimeAB::~AM1DDeadTimeAB(){}
AM1DDeadTimeAB::AM1DDeadTimeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectra_ = 0;
	icr_ = 0;
	ocr_ = 0;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

bool AM1DDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise there are three data sources with rank 1.
	if (dataSources.count() == 3
			&& dataSources.at(0)->rank() == 1
			&& dataSources.at(1)->rank() == 1
			&& dataSources.at(2)->rank() == 1)
		return true;

	return false;
}

AMNumber AM1DDeadTimeAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (indexes.i() >= spectra_->size(0))
		return AMNumber(AMNumber::OutOfBoundsError);

	if ((int)spectra_->value(indexes) == 0 || double(ocr_->value(indexes.i())) == 0)
		return 0;
	else
		return double(icr_->value(indexes.i()))/double(ocr_->value(indexes.i()))*(int)spectra_->value(indexes.i());
}

bool AM1DDeadTimeAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;

	int totalSize = indexStart.totalPointsTo(indexEnd);

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> icr = QVector<double>(totalSize);
	QVector<double> ocr = QVector<double>(totalSize);
	spectra_->values(indexStart, indexEnd, data.data());
	icr_->values(indexStart, indexEnd, icr.data());
	ocr_->values(indexStart, indexEnd, ocr.data());

	for (int i = 0; i < totalSize; i++){

		// If ocr is equal to 0 then that will cause division by zero.  Since these are both count rates, they should both be greater than zero.
		if (icr.at(i) <= 0 || ocr.at(i) <= 0)
			outputValues[i] = 0;

		else
			outputValues[i] = data.at(i)*icr.at(i)/ocr.at(i);
	}

	return true;
}

AMNumber AM1DDeadTimeAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index >= spectra_->size(0))
		return AMNumber(AMNumber::OutOfBoundsError);

	return spectra_->axisValue(axisNumber, index);
}

bool AM1DDeadTimeAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return spectra_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM1DDeadTimeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	if (start.rank() == axes_.size() && end.rank() == axes_.size())
		emitValuesChanged(start, end);

	else
		emitValuesChanged();
}

void AM1DDeadTimeAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != spectra_->size(0)){

		axes_[0].size = spectra_->size(0);
		emitSizeChanged(0);
	}
}

void AM1DDeadTimeAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
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
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		spectra_ = dataSources.first();
		icr_ = dataSources.at(1);
		ocr_ = dataSources.at(2);

		sources_ = dataSources;

		axes_[0] = spectra_->axisInfoAt(0);

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
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM1DDeadTimeAB::reviewState()
{
	if (spectra_ == 0 || icr_ == 0 || ocr_ == 0 || !spectra_->isValid() || !icr_->isValid() || !ocr_->isValid()){

		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}
