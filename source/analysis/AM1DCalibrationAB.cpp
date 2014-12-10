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


#include <QDebug>
#include <QtCore/qmath.h>

#include "AM1DCalibrationAB.h"


AM1DCalibrationAB::AM1DCalibrationAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	normalizer_ = 0;
	canAnalyze_ = false;
	dataName_ = "";
	normalizationName_ = "";
	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);

	energyCalibrationOffset_ = 0;
	energyCalibrationScaling_ = 1;
	energyCalibrationReference_ = 0;
	isTransmission_ = false;
	toEdgeJump_ = false;
	preEdgePoint_ = -1;
	postEdgePoint_ = -1;

}

bool AM1DCalibrationAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need two input sources, with a rank of 1.
	if(dataSources.count() == 2 && dataSources.at(0)->rank() == 1 && dataSources.at(1)->rank() == 1)
		return true;

	// Otherwise, any number of data sources (greater than 2) that all have a rank of 1.
	if (dataSources.count() > 2){

		for (int i = 0; i < dataSources.size(); i++)
			if (dataSources.at(i)->rank() != 1)
				return false;

		return true;
	}

	return false;
}

void AM1DCalibrationAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (normalizer_){

		disconnect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		normalizer_ = 0;
	}

	if (dataSources.isEmpty()){

		data_ = 0;
		normalizer_ = 0;
		sources_.clear();
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Normalized 1D Data Source");
	}

	else if (dataSources.count() == 2){

		data_ = dataSources.at(0);
		normalizer_ = dataSources.at(1);
		sources_ = dataSources;
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);

		setDescription(QString("Normalized %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else {

		sources_ = dataSources;
		setInputSources();
	}

	reviewState();


	setModified(true);
	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM1DCalibrationAB::setDataName(const QString &name)
{
	dataName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
	emitValuesChanged();
}

void AM1DCalibrationAB::setNormalizationName(const QString &name)
{
	normalizationName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
	emitValuesChanged();
}

void AM1DCalibrationAB::setEnergyCalibrationOffset(double offset)
{
	if(offset == energyCalibrationOffset_)
		return;
	energyCalibrationOffset_ = offset;
	emitSizeChanged(0);
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setEnergyCalibrationScaling(double scaling)
{
	if(scaling == energyCalibrationScaling_)
		return;

	energyCalibrationScaling_ = scaling;
	emitSizeChanged(0);
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setEnergyCalibrationReference(double reference)
{
	if(reference == energyCalibrationReference_)
		return;

	energyCalibrationReference_ = reference;
	emitSizeChanged(0);
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setIsTransmission(bool isTransmission)
{
	if(isTransmission == isTransmission_)
		return;

	isTransmission_ = isTransmission;
	setModified(true);
	emitValuesChanged();
}


void AM1DCalibrationAB::setToEdgeJump(bool toEdgeJump)
{
	if(toEdgeJump == toEdgeJump_)
		return;

	toEdgeJump_ = toEdgeJump;
	qDebug() << "toEdgeJump is now" << toEdgeJump_;
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setPreEdgePoint(int preEdgePoint)
{
	if(preEdgePoint == preEdgePoint_)
		return;

	preEdgePoint_ = preEdgePoint;
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setPostEdgePoint(int postEdgePoint)
{
	if(postEdgePoint == postEdgePoint_)
		return;

	postEdgePoint_ = postEdgePoint;
	setModified(true);
	emitValuesChanged();
}

void AM1DCalibrationAB::setInputSources()
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (normalizer_){

		disconnect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		normalizer_ = 0;
	}

	int dataIndex = indexOfInputSource(dataName_);
	int NormalizationIndex = indexOfInputSource(normalizationName_);

	if (dataIndex >= 0 && NormalizationIndex >= 0){

		data_ = inputDataSourceAt(dataIndex);
		normalizer_ = inputDataSourceAt(NormalizationIndex);
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);

		setDescription(QString("Normalized %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		data_ = 0;
		normalizer_ = 0;
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Normalized 1D Data Source");
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

bool AM1DCalibrationAB::canAnalyze(const QString &dataName, const QString &NormalizationName) const
{
	// Can always analyze two 1D data sources.
	if (sources_.count() == 2)
		return true;

	// The first data source is reserved for the data.
	if (indexOfInputSource(dataName) >= 0 && indexOfInputSource(NormalizationName))
		return true;

	return false;
}








AMNumber AM1DCalibrationAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif


	double retVal;
	int totalPoints = data_->size(0);


	if(isTransmission_)
	{
		if (double(normalizer_->value(indexes)) == 0)
				retVal = 0;

		else if (double(normalizer_->value(indexes)) * double(data_->value(indexes)) < 0)  //Don't take the log of a negative number
			retVal = 0;

		else
			retVal = qLn(double(normalizer_->value(indexes))/double(data_->value(indexes)));
	}
	else
	{
		if (double(normalizer_->value(indexes)) == 0)  // don't divide by zero
			retVal = 0;

		else if (double(normalizer_->value(indexes)) < 0) // Not sure, I didn't put it here (DM)
			retVal = -1;

		else
			retVal = double(data_->value(indexes))/double(normalizer_->value(indexes));
	}

	if(toEdgeJump_)
	{
		int preEdgePointValue, postEdgePointValue;

		// Determine a safe preEdgePoint
		if(preEdgePoint() < 0 or preEdgePoint() > totalPoints -1)
		{
			preEdgePointValue = 0;
			if(totalPoints > 1)  preEdgePointValue = 1; //Don't use the first point by defualt, because it's too-often bad.
		}
		else
			preEdgePointValue = preEdgePoint();

		// Determine a safe totalPoints
		if(postEdgePoint() < 0 or postEdgePoint() > totalPoints -1)
			postEdgePointValue = totalPoints - 1;
		else
			postEdgePointValue = postEdgePoint();

		// determine value of desired offset reference point
		double offset = int(data_->value(preEdgePointValue));
		// shift spectra so that pre edge reference point is 0
			retVal = retVal - offset;

		// determine value of post edge scaling reference point
		double scale = data_->value(postEdgePointValue);
		// scale spectra so that post edge reference point is 1
		if (scale != 0){ // if the postEdgePoint is zero, do nothing, otherwise scale and shift the output values
			retVal = retVal / scale;

		}
	}
	return retVal;
}







bool AM1DCalibrationAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

	int totalSize = indexStart.totalPointsTo(indexEnd);
	int totalPoints = data_->size(0);

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> normalizer = QVector<double>(totalSize);

	data_->values(indexStart, indexEnd, data.data());
	normalizer_->values(indexStart, indexEnd, normalizer.data());

	for (int i = 0; i < totalSize; i++){

		if(isTransmission())
		{
			if (data.at(i) == 0) // don't divide by zero
				outputValues[i] = 0;

			else if (normalizer.at(i) * data.at(i) < 0)  //Don't take the log of a negative number
				outputValues[i] = 0;

			else
				outputValues[i] = qLn(normalizer.at(i)/data.at(i));
		}
		else
		{
			if (normalizer.at(i) == 0)  // don't divide by zero
				outputValues[i] = 0;

			else if (normalizer.at(i) < 0) // Not sure, I didn't put it here (DM)
				outputValues[i] = -1;

			else
				outputValues[i] = data.at(i)/normalizer.at(i);
		}
	}

	if(toEdgeJump_)
	{
		int preEdgePointValue, postEdgePointValue;

		// Determine a safe preEdgePoint
		if(preEdgePoint_ < 0 or preEdgePoint_ > totalPoints -1)
		{
			preEdgePointValue = 0;
			if(totalPoints > 1)  preEdgePointValue = 1; //Don't use the first point by defualt, because it's too-often bad.
		}
		else
			preEdgePointValue = preEdgePoint_;

		// Determine a safe postEdgePoint
		if(postEdgePoint() < 0 or postEdgePoint() > totalPoints -1)
			postEdgePointValue = totalPoints - 1;
		else
			postEdgePointValue = postEdgePoint_;

		// determine value of desired offset reference point, this is ugly because it must handle the case that the reference point is outside the requested index range for all modes...
		double offset;
		if(isTransmission())
		{
			if (double(data_->value(preEdgePointValue)) == 0) // don't divide by zero
				offset = 0;

			else if (double(normalizer_->value(preEdgePointValue)) * double(data_->value(preEdgePointValue)) < 0)  //Don't take the log of a negative number
				offset = 0;

			else
				offset = qLn(double(normalizer_->value(preEdgePointValue))/double(data_->value(preEdgePointValue)));
		}
		else
		{
			if (double(normalizer_->value(preEdgePointValue)) == 0)  // don't divide by zero
				offset = 0;
			else
				offset = double(data_->value(preEdgePointValue))/double(normalizer_->value(preEdgePointValue));
		}



		qDebug() << "the choosen pre edge point is" << preEdgePointValue << " with a value of" << offset  ;
		// shift spectra so that pre edge reference point is 0
		for (int i = 0; i < totalSize; i++)
			outputValues[i] = outputValues[i] - offset;

		// determine value of post edge scaling reference point, this is ugly because it must handle the case that the reference point is outside the requested index range for all modes...
		double scale;
		if(isTransmission())
		{
			if (double(data_->value(postEdgePointValue)) == 0) // don't divide by zero
				scale = 1;

			else if (double(normalizer_->value(postEdgePointValue)) * double(data_->value(postEdgePointValue)) < 0)  //Don't take the log of a negative number
				scale = 1;

			else
				scale = qLn(double(normalizer_->value(postEdgePointValue))/double(data_->value(postEdgePointValue))) - offset;
		}
		else
		{
			if (double(normalizer_->value(postEdgePointValue)) == 0)  // don't divide by zero
				scale = 1;

			else
				scale = (double(data_->value(postEdgePointValue))/double(normalizer_->value(postEdgePointValue))) - offset;
		}


		// scale spectra so that post edge reference point is 1
		if (scale != 0){ // if the postEdgePoint is zero, do nothing, otherwise scale and shift the output values
			for (int i = 0; i < totalSize; i++)
			{
				outputValues[i] = outputValues[i] / scale;
			}
		}
	}



	return true;
}

AMNumber AM1DCalibrationAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return AMNumber(energyCalibrationReference_ + ((double(data_->axisValue(axisNumber, index)) - energyCalibrationReference_) * energyCalibrationScaling_) + energyCalibrationOffset_ );
}

bool AM1DCalibrationAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	double energyCalibrationOffsetAndReference = (1 - energyCalibrationScaling_)*energyCalibrationReference_ + energyCalibrationOffset_;
	int totalSize = endIndex - startIndex + 1;
	QVector<AMNumber> axisData = QVector<AMNumber>(totalSize, 0);
	data_->axisValues(axisNumber, startIndex, endIndex, axisData.data());

	for (int i = 0; i < totalSize; i++)
		outputValues[i] = AMNumber(double(axisData.at(i))*energyCalibrationScaling_ + energyCalibrationOffsetAndReference);

	return true;
}

void AM1DCalibrationAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AM1DCalibrationAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != data_->size(0)){

		axes_[0].size = data_->size(0);
		emitSizeChanged(0);
	}
}

void AM1DCalibrationAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DCalibrationAB::reviewState(){

	if(!canAnalyze_
			|| data_ == 0 || !data_->isValid()
			|| normalizer_ == 0 || !normalizer_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

#include "analysis/AM1DCalibrationABEditor.h"

QWidget *AM1DCalibrationAB::createEditorWidget()
{
	return new AM1DCalibrationABEditor(this);
}

bool AM1DCalibrationAB::loadFromDb(AMDatabase *db, int id) {
	bool success = AMDbObject::loadFromDb(db, id);
	if(success)
		AMDataSource::name_ = AMDbObject::name();	/// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
	return success;
}
