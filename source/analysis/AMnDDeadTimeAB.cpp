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


#include "AMnDDeadTimeAB.h"

 AMnDDeadTimeAB::~AMnDDeadTimeAB(){}
AMnDDeadTimeAB::AMnDDeadTimeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectrum_ = 0;
	inputCounts_ = 0;
	outputCounts_ = 0;

	setState(AMDataSource::InvalidFlag);
}

bool AMnDDeadTimeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise there are three data sources
	if (dataSources.size() == 3){

		int spectrumRank = dataSources.at(0)->rank();

		bool isAcceptable = (spectrumRank >= 1);
		isAcceptable &= (dataSources.at(1)->rank() == spectrumRank-1);
		isAcceptable &= (dataSources.at(2)->rank() == spectrumRank-1);

		return isAcceptable;
	}

	return false;
}

AMNumber AMnDDeadTimeAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != rank())
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexes.at(i) >= axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);

	if ((int)spectrum_->value(indexes) == 0 || double(outputCounts_->value(indexes.i())) == 0)
		return 0;
	else
		return double(inputCounts_->value(indexes.i()))/double(outputCounts_->value(indexes.i()))*(int)spectrum_->value(indexes);
}

bool AMnDDeadTimeAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != rank() || indexEnd.rank() != indexStart.rank())
		return false;

	if(!isValid())
		return false;

	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexEnd.at(i) >= axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;

	switch(rank()){

	case 0:	// Can't happen.
		break;

	case 1:{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		double inputCounts = inputCounts_->value(AMnDIndex());
		double outputCounts = outputCounts_->value(AMnDIndex());

		if (outputCounts == 0){

			QVector<double> data = QVector<double>(totalSize, 0);
			outputValues = data.data();
		}

		else {

			double scalingFactor = qAbs(inputCounts/outputCounts);

			QVector<double> data = QVector<double>(totalSize);
			spectrum_->values(indexStart, indexEnd, data.data());

			for (int i = 0, size = data.size(); i < size; i++)
				outputValues[i] = data.at(i)*scalingFactor;
		}

		break;
	}

	case 2:{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		int crTotalSize = AMnDIndex(indexStart.i()).totalPointsTo(AMnDIndex(indexEnd.i()));

		QVector<double> data = QVector<double>(totalSize);
		QVector<double> inputCounts = QVector<double>(crTotalSize);
		QVector<double> outputCounts = QVector<double>(crTotalSize);
		spectrum_->values(indexStart, indexEnd, data.data());
		inputCounts_->values(indexStart.i(), indexEnd.i(), inputCounts.data());
		outputCounts_->values(indexStart.i(), indexEnd.i(), outputCounts.data());

		for (int i = 0, iSize = indexEnd.i() - indexStart.i()+1; i < iSize; i++){

			// If outputCounts is equal to 0 then that will cause division by zero.
			if (outputCounts.at(i) <= 0){

				for (int j = 0, jSize = indexEnd.j()-indexStart.j()+1; j < jSize; j++)
					outputValues[i*jSize+j] = 0;
			}

			else {

				double factor = qAbs(inputCounts.at(i)/outputCounts.at(i));

				for (int j = 0, jSize = indexEnd.j()-indexStart.j()+1; j < jSize; j++)
					outputValues[i*jSize+j] = data.at(i*jSize+j)*factor;
			}
		}

		break;
	}

	case 3:{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		AMnDIndex start2D = AMnDIndex(indexStart.i(), indexStart.j());
		AMnDIndex end2D = AMnDIndex(indexEnd.i(), indexEnd.j());
		int icrOcrTotalSize = start2D.totalPointsTo(end2D);

		QVector<double> data = QVector<double>(totalSize);
		QVector<double> inputCounts = QVector<double>(icrOcrTotalSize);
		QVector<double> outputCounts = QVector<double>(icrOcrTotalSize);
		spectrum_->values(indexStart, indexEnd, data.data());
		inputCounts_->values(start2D, end2D, inputCounts.data());
		outputCounts_->values(start2D, end2D, outputCounts.data());

		for (int i = 0, iSize = indexEnd.i()-indexStart.i()+1; i < iSize; i++){

			for (int j = 0, jSize = indexEnd.j()-indexStart.j()+1; j < jSize; j++){

				int scaleFactorIndex = i*jSize+j;

				// If outputCounts is equal to 0 then that will cause division by zero.
				if (outputCounts.at(scaleFactorIndex) <= 0){

					for (int k = 0, kSize = indexEnd.k()-indexStart.k()+1; k < kSize; k++)
						outputValues[i*jSize*kSize+j*kSize+k] = 0;
				}

				else {

					double scaleFactor = qAbs(inputCounts.at(scaleFactorIndex)/outputCounts.at(scaleFactorIndex));

					for (int k = 0, kSize = indexEnd.k()-indexStart.k()+1; k < kSize; k++){

						int spectrumIndex = i*jSize*kSize+j*kSize+k;
						outputValues[spectrumIndex] = data.at(spectrumIndex)*scaleFactor;
					}
				}
			}
		}

		break;
	}

	case 4:{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		AMnDIndex start3D = AMnDIndex(indexStart.i(), indexStart.j());
		AMnDIndex end3D = AMnDIndex(indexEnd.i(), indexEnd.j());
		int icrOcrTotalSize = start3D.totalPointsTo(end3D);

		QVector<double> data = QVector<double>(totalSize);
		QVector<double> inputCounts = QVector<double>(icrOcrTotalSize);
		QVector<double> outputCounts = QVector<double>(icrOcrTotalSize);
		spectrum_->values(indexStart, indexEnd, data.data());
		inputCounts_->values(start3D, end3D, inputCounts.data());
		outputCounts_->values(start3D, end3D, outputCounts.data());

		for (int i = 0, iSize = indexEnd.i()-indexStart.i()+1; i < iSize; i++){

			for (int j = 0, jSize = indexEnd.j()-indexStart.j()+1; j < jSize; j++){

				for (int k = 0, kSize = indexEnd.k()-indexStart.k()+1; k < kSize; k++){

					int scaleFactorIndex = i*jSize*kSize+j*kSize+k;

					// If outputCounts is equal to 0 then that will cause division by zero.
					if (outputCounts.at(scaleFactorIndex) <= 0){

						for (int l = 0, lSize = indexEnd.l()-indexStart.l()+1; l < lSize; l++)
							outputValues[i*jSize*kSize*lSize+j*kSize*lSize+k*lSize+l] = 0;
					}

					else {

						double scaleFactor = qAbs(inputCounts.at(scaleFactorIndex)/outputCounts.at(scaleFactorIndex));

						for (int l = 0, lSize = indexEnd.l()-indexStart.l()+1; l < lSize; l++){

							int spectrumIndex = i*jSize*kSize*lSize+j*kSize*lSize+k*lSize+l;
							outputValues[spectrumIndex] = data.at(spectrumIndex)*scaleFactor;
						}
					}
				}
			}
		}

		break;
	}
	}

	return true;
}

AMNumber AMnDDeadTimeAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber < 0 && axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

	if (index < 0 || index >= spectrum_->size(rank()-1))
		return AMNumber(AMNumber::OutOfBoundsError);

	return spectrum_->axisValue(axisNumber, index);
}

bool AMnDDeadTimeAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if(!isValid())
		return false;

	if(axisNumber < 0 || axisNumber >= rank())
		return false;

	if (startIndex < 0 || startIndex >= spectrum_->size(axisNumber) || endIndex < 0 || endIndex >= spectrum_->size(axisNumber))
		return false;

	return spectrum_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}


void AMnDDeadTimeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	if (start.rank() == axes_.size() && end.rank() == axes_.size())
		emitValuesChanged(start, end);

	else
		emitValuesChanged();
}

void AMnDDeadTimeAB::onInputSourceSizeChanged()
{
	for (int i = 0, size = rank(); i < size; i++){

			axes_[i] = spectrum_->axisInfoAt(i);
			emitSizeChanged(i);
	}
}

void AMnDDeadTimeAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AMnDDeadTimeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(spectrum_ != 0 && inputCounts_ != 0 && outputCounts_ != 0) {

		disconnect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectrum_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		disconnect(inputCounts_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputCounts_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputCounts_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputCounts_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		disconnect(outputCounts_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(outputCounts_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(outputCounts_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(outputCounts_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		spectrum_ = 0;
		inputCounts_ = 0;
		outputCounts_ = 0;
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_.clear();
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		spectrum_ = dataSources.first();
		inputCounts_ = dataSources.at(1);
		outputCounts_ = dataSources.at(2);

		sources_ = dataSources;

		foreach (AMAxisInfo axis, spectrum_->axes())
			axes_.append(axis);

		setDescription(QString("Corrected %1").arg(spectrum_->description()));

		connect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectrum_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(inputCounts_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputCounts_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputCounts_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputCounts_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		connect(outputCounts_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(outputCounts_->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(outputCounts_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(outputCounts_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMnDDeadTimeAB::reviewState()
{
	if (spectrum_ == 0 || inputCounts_ == 0 || outputCounts_ == 0
			|| !spectrum_->isValid() || !inputCounts_->isValid() || !outputCounts_->isValid()){

		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}
