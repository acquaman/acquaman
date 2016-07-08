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


#include "REIXSXESImageInterpolationAB.h"

#include "util/AMErrorMonitor.h"
#include "util/AMUtility.h"

REIXSXESImageInterpolationAB::REIXSXESImageInterpolationAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	// Live correlation turned off by default. Prevents correlation while loading from db and no shift is really better than a random shift, initially.
	liveCorrelation_ = false;

	curve1Smoother_ = 0;
	curve2Smoother_ = 0;

	sumRangeMinY_ = 10;
	sumRangeMaxY_ = 53;
	sumRangeMinX_ = 75;
	sumRangeMaxX_ = 925;
	rangeRound_ = 0.6;
	curve2Disabled_ = false;
	correlation1CenterPx_ = 250;
	correlation2CenterPx_ = 700;
	correlation1HalfWidth_ = 40;
	correlation2HalfWidth_ = 40;
	setCorrelation1Smoothing(QPair<int,int>(3,5)); //poly,quadratic
	setCorrelation2Smoothing(QPair<int,int>(3,5)); //poly,quadratic

	energyCalibrationOffset_ = 0;
	tiltCalibrationOffset_ = 0;

	interpolationLevel_ = 10;
	binningLevel_ = 2;

	inputSource_ = 0;
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();
	axisValueCacheInvalid_ = true;
	axisValuesInvalid_ = false;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
	setDescription("XES Interpolated Spectrum");

	// shift values can start out empty.
	shiftValues1_ << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	shiftValues2_ << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;

}

REIXSXESImageInterpolationAB::~REIXSXESImageInterpolationAB() {
}


AMDoubleList REIXSXESImageInterpolationAB::shiftValuesAt(int i)
{
	if(cacheUpdateRequired_)
		computeCachedValues();

	AMDoubleList shiftValuesAt;

	int jSize = inputSource_->size(1); //64
	int interpolatedI = i*interpolationLevel_;
	double *shiftValueMapPointer = lastShiftValueMap_.data();

	for(int j = 0; j < jSize; j++)
	{
		shiftValuesAt << shiftValueMapPointer[interpolatedI*jSize+j];
	}



	return shiftValuesAt;
}



void REIXSXESImageInterpolationAB::setSumRangeMinY(int sumRangeMinY)
{
	// no change
	if(sumRangeMinY == sumRangeMinY_)
		return;

	sumRangeMinY_ = sumRangeMinY;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	cacheUpdateRequired_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setSumRangeMaxY(int sumRangeMaxY)
{
	if(sumRangeMaxY == sumRangeMaxY_)
		return;

	sumRangeMaxY_ = sumRangeMaxY;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	cacheUpdateRequired_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setSumRangeMinX(int sumRangeMinX)
{
	// no change
	if(sumRangeMinX == sumRangeMinX_)
		return;

	sumRangeMinX_ = sumRangeMinX;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	cacheUpdateRequired_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setSumRangeMaxX(int sumRangeMaxX)
{
	if(sumRangeMaxX == sumRangeMaxX_)
		return;

	sumRangeMaxX_ = sumRangeMaxX;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	cacheUpdateRequired_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}



void REIXSXESImageInterpolationAB::setShiftValues1(const AMIntList &shiftValues1)
{
	if(shiftValues1 == shiftValues1_)
		return;

	shiftValues1_ = shiftValues1;
	cacheUpdateRequired_ = true;	// could change all our cached values
	reviewState();	// might change the state to valid, if we didn't have proper number of shift values before.

	emitValuesChanged();
	emit shiftValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setShiftValues2(const AMIntList &shiftValues2)
{
	if(shiftValues2 == shiftValues2_)
		return;
	shiftValues2_ = shiftValues2;

	cacheUpdateRequired_ = true;	// could change all our cached values
	reviewState();	// might change the state to valid, if we didn't have proper number of shift values before.

	emitValuesChanged();
	emit shiftValuesChanged();
	setModified(true);
}


void REIXSXESImageInterpolationAB::setRangeRound(double rangeRound)
{
	if(rangeRound == rangeRound_)
		return;

	rangeRound_ = rangeRound;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	cacheUpdateRequired_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::reviewState()
{

	if(inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	int s = inputSource_->size(1);

	if(sumRangeMinY_ >= s || sumRangeMaxY_ >= s) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	if(sumRangeMinY_ > sumRangeMaxY_) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	if(shiftValues1_.size() != s && shiftValues2_.size() != s) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	setState(0);

}


bool REIXSXESImageInterpolationAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if(dataSources.isEmpty())
		return true;// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 2)
		return true;

	return false;
}

void REIXSXESImageInterpolationAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
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

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
	}

	// we know that this will only be called with one valid input source
	else {
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;

		axes_[0] = inputSource_->axisInfoAt(0);	// take the X axis info from the data source.
		axes_[0].description = "Emission Energy";
		axes_[0].units = "eV";
		axes_[0].size /= binningLevel_;

		cacheUpdateRequired_ = true;
		axisValueCacheInvalid_ = true;
		cachedData_ = QVector<double>(axes_.at(0).size);
		cachedAxisValues_ = QVector<double>(axes_.at(0).size);

		if(shiftValues1_.isEmpty()) {
			for(int i=0,cc=inputSource_->size(1); i<cc; i++)
				shiftValues1_ << 0;	// make a null shift list, matching the size of the input data. Don't do this if we already have a shift list.
		}
		if(shiftValues2_.isEmpty()) {
			for(int i=0,cc=inputSource_->size(1); i<cc; i++)
				shiftValues2_ << 0;	// make a null shift list, matching the size of the input data. Don't do this if we already have a shift list.
		}


		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}


AMNumber REIXSXESImageInterpolationAB::value(const AMnDIndex &indexes) const
{
	if((indexes.rank() != 1))
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(((unsigned long)indexes.i() >= (unsigned long)axes_.at(0).size))
		return AMNumber(AMNumber::OutOfBoundsError);

	if(cacheUpdateRequired_)
		computeCachedValues();

	return AMNumber(cachedData_.at(indexes.i()));
}

bool REIXSXESImageInterpolationAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;	// dimension error

	if(!isValid())
		return false; // invalid error

	// Max x pixel value:
	int maxI = inputSource_->size(0);

	if(indexEnd.i() < indexStart.i())
		return false;
	if(indexEnd.i() >= maxI)
		return false;

	if(cacheUpdateRequired_)
		computeCachedValues();

	memcpy(outputValues, (cachedData_.constData()+indexStart.i()), (indexEnd.i()-indexStart.i()+1)*sizeof(double));

	return true;
}

void REIXSXESImageInterpolationAB::computeCachedValues() const
{
	int iSize = inputSource_->size(0); //1024
	int jSize = inputSource_->size(1); //64

	// grab the full image
	QVector<double> image(iSize*jSize);
	if(!inputSource_->values(AMnDIndex(0,0), AMnDIndex(iSize-1, jSize-1), image.data())) {
		AMErrorMon::report(this, AMErrorReport::Alert, -333, "Could not retrieve values from detector image. Please report this problem to the REIXS Acquaman developers.");
		cacheUpdateRequired_ = false;	// avoid repeating this error message for every single data point...
		return;
	}

	//If there is on one shift value, don't interpolate.
	if(shiftValues2().isEmpty() || curve2Disabled_)
	{
		//The center point of the sum region
		double originX = (double)sumRangeMinX_ + ((double)sumRangeMaxX_ - (double)sumRangeMinX_)/2.0;
		double originY = (double)sumRangeMinY_ + ((double)sumRangeMaxY_ - (double)sumRangeMinY_)/2.0;
		//Width and height of the sum region, in pixels
		double sumRangeWidth = (double)(sumRangeMaxX_ - sumRangeMinX_);
		double sumRangeHeight = (double)(sumRangeMaxY_ - sumRangeMinY_);


		for(int bin = 0; bin < iSize / binningLevel_; bin++) {
			cachedData_[bin] = 0;
			for(int temp = 0; temp < binningLevel_; temp++ ) {
				int i = bin * binningLevel_ + temp;
				double newVal = 0.0;
				int contributingRows = 0;
				if(i > sumRangeMinX_ && i < sumRangeMaxX_) {
					double xVal = (double)i - originX;
					for(int j=sumRangeMinY_; j<=sumRangeMaxY_; j++) { // loop through rows
						if(rangeRound_ == 0.0) { //not ellipse
							int sourceI = i + shiftValues1_.at(j);
							if(sourceI < iSize && sourceI >= 0) {
								newVal += image.at(sourceI*jSize + j);
								contributingRows++;
							}
						}
						else {

							double yVal = (double)j - originY;
							if(isWithinMaskEllipse(xVal, yVal, sumRangeWidth, sumRangeHeight)) {
								int sourceI = i + shiftValues1_.at(j);
								if(sourceI < iSize && sourceI >= 0) {
									newVal += image.at(sourceI*jSize + j);
									contributingRows++;
								}
							}
						}
					}
				}
				// normalize by dividing by the number of rows that contributed. Since we want to keep the output in units similar to raw counts, multiply by the nominal (usual) number of contributing rows.
				// Essentially, this normalization prevents columns near the edge that miss out on some rows due to shifting from being artificially suppressed.  For inner columns, contributingRows will (sumRangeMax_ - sumRangeMin_ + 1).
				if(contributingRows == 0)
					newVal = 0;
				else
					newVal = newVal * double(sumRangeMaxY_ - sumRangeMinY_ + 1) / double(contributingRows);

				cachedData_[bin] = cachedData_[bin] + newVal;
			}
		}
	}
	else
	{
		int interpolatedISize = (iSize-1)*interpolationLevel_;
		QVector<double> shiftValueMap = QVector<double>(interpolatedISize*jSize, 0);
		computeShiftMap(interpolatedISize, jSize, shiftValueMap.data());
		double *shiftValueMapPointer = shiftValueMap.data();

		//interpolate image in x, probably rotating image by 90 degree in memory because of a switch in conventions...
		QVector<double> interpolatedImage = QVector<double>(interpolatedISize*jSize, 0);
		double *interpolatedImagePointer = interpolatedImage.data();

		for(int i = 0, modifiedISize = iSize-1; i < modifiedISize; i++){

			for(int j = 0; j < jSize; j++){

				int index = j + i*jSize;
				int interpolatedIndex = j + i*jSize*interpolationLevel_;
				double imageValue = image.at(index);
				double nextImageValue = image.at(j + (i+1)*jSize);

				for(int k = 0; k < interpolationLevel_; k++)
					interpolatedImagePointer[interpolatedIndex + k*jSize] = imageValue + ((double)k / (double)interpolationLevel_) * (nextImageValue - imageValue);
			}
		}

		//OKAY!  Now I think we have a shiftMap_ and a interpolatedImage that mirror those used in Robert's code.
		//***************************************************************************
		//***************************************************************************

		QVector<double> finalLargeImage = QVector<double>(interpolatedISize*jSize, 0);
		double *finalLargeImagePointer = finalLargeImage.data();

		//iterate through shiftMap, adding the corresponding element from interpolated to the shifted location in this new array
		for(int i = 0; i < interpolatedISize; i++){

			for(int j = 0; j < jSize; j++){

				int shiftOffset = qRound(shiftValueMapPointer[i*jSize+j] * interpolationLevel_);

				if (((i + shiftOffset) < interpolatedISize) && ((i + shiftOffset) > 0))
				{
					//add only one pixel from the interpolated image to each pixel in the shifted image:
					finalLargeImagePointer[j + i*jSize] = interpolatedImagePointer[j + (i + shiftOffset)*jSize];
				}
			}
		}

		QVector<double> tempFinalVector = QVector<double>(iSize*jSize, 0);
		double *tempFinalVectorPointer = tempFinalVector.data();

		for(int i = 0, modifiedISize = iSize-1; i < modifiedISize; i++){

			for(int j = 0; j < jSize; j++){

				int offset = j + i*jSize;
				int largeOffset = j + i*jSize*interpolationLevel_;

				for (int k = 0; k < interpolationLevel_; k++)
					tempFinalVectorPointer[offset] += finalLargeImagePointer[largeOffset + k*jSize];

				tempFinalVectorPointer[offset] /= double(interpolationLevel_);
			}
		}

//		***************************************************************************
//		***************************************************************************

		//The center point of the sum region
		double originX = (double)sumRangeMinX_ + ((double)sumRangeMaxX_ - (double)sumRangeMinX_)/2.0;
		double originY = (double)sumRangeMinY_ + ((double)sumRangeMaxY_ - (double)sumRangeMinY_)/2.0;
		//Width and height of the sum region, in pixels
		double sumRangeWidth = (double)(sumRangeMaxX_ - sumRangeMinX_);
		double sumRangeHeight = (double)(sumRangeMaxY_ - sumRangeMinY_);


		for(int bin = 0; bin < iSize / binningLevel_; bin++) {
			cachedData_[bin] = 0;
			for(int temp = 0; temp < binningLevel_; temp++ ) {
				int i = bin * binningLevel_ + temp;
				double newVal = 0.0;
				int contributingRows = 0;

				if(i > sumRangeMinX_ && i < sumRangeMaxX_) {

					double xVal = (double)i - originX;

					for (int j = sumRangeMinY_; j <= sumRangeMaxY_; j++) { // loop through rows

						if (rangeRound_ == 0.0) { //not ellipse

							newVal += tempFinalVectorPointer[j+i*jSize];
							contributingRows++;
						}

						else {

							double yVal = (double)j - originY;

							if(isWithinMaskEllipse(xVal, yVal, sumRangeWidth, sumRangeHeight)) {

								newVal += tempFinalVectorPointer[j+i*jSize];
								contributingRows++;
							}
						}
					}
				}
				// normalize by dividing by the number of rows that contributed. Since we want to keep the output in units similar to raw counts, multiply by the nominal (usual) number of contributing rows.
				// Essentially, this normalization prevents columns near the edge that miss out on some rows due to shifting from being artificially suppressed.  For inner columns, contributingRows will (sumRangeMax_ - sumRangeMin_ + 1).
				if(contributingRows == 0)
					newVal = 0;

				else
					newVal = newVal * double(sumRangeMaxY_ - sumRangeMinY_ + 1) / double(contributingRows);

				cachedData_[bin] += newVal;
			}
		}
	}

	cachedDataRange_ = AMUtility::rangeFinder(cachedData_);
	cacheUpdateRequired_ = false;
}

void REIXSXESImageInterpolationAB::computeShiftMap(int iSize, int jSize, double *shiftValues) const
{

	int newShiftMapSize = (iSize-1)*interpolationLevel_*jSize;

	if (newShiftMapSize != lastShiftValueMap_.size())
		lastShiftValueMap_ = QVector<double>(newShiftMapSize, 0);

	double *lastShiftValueMapPointer = lastShiftValueMap_.data();

	double weightingValue = double(interpolationLevel_*(correlation1CenterPx_-correlation2CenterPx_));

	int interpolatedPosition1 = interpolationLevel_*correlation1CenterPx_;
	for (int i = 0; i < iSize; i++){

		for (int j = 0; j < jSize; j++){

			int shiftValue1 = shiftValues1_.at(j);
			shiftValues[i*jSize + j] = ((shiftValue1 - shiftValues2_.at(j)) * double((i-interpolatedPosition1))) / weightingValue + shiftValue1;
			lastShiftValueMapPointer[i*jSize + j] = shiftValues[i*jSize + j];


		}
	}

//	memcpy(lastShiftValueMap_.data(), shiftValues, newShiftMapSize*sizeof(double));

}

AMNumber REIXSXESImageInterpolationAB::axisValue(int axisNumber, int index) const
{
	if((axisNumber != 0))
		return AMNumber(AMNumber::DimensionError);

	if(((unsigned)index >= (unsigned)axes_.at(0).size))
		return AMNumber(AMNumber::OutOfBoundsError);

	if(axisValueCacheInvalid_)
		computeCachedAxisValues();

	if(axisValuesInvalid_)
		return index;

	return cachedAxisValues_.at(index);
}

bool REIXSXESImageInterpolationAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (axisNumber != 0)
		return false;

	if (!isValid())
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	if(axisValueCacheInvalid_)
		computeCachedAxisValues();

	int totalSize = endIndex-startIndex+1;

	if(axisValuesInvalid_){

		for (int i = 0; i < totalSize; i++)
			outputValues[i] = startIndex+i;
	}

	else
		memcpy(outputValues, cachedAxisValues_.constData()+startIndex, (endIndex-startIndex+1)*sizeof(double));

	return true;
}

void REIXSXESImageInterpolationAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	// invalidate the cache.
	cacheUpdateRequired_ = true;
	emitValuesChanged();
}

void REIXSXESImageInterpolationAB::onInputSourceSizeChanged()
{
	cacheUpdateRequired_ = true;
	axisValueCacheInvalid_ = true;

	int axisSize = inputSource_->size(0)/binningLevel_;
	axes_[0].size = axisSize;
	cachedData_ = QVector<double>(axisSize);
	cachedAxisValues_ = QVector<double>(axisSize);

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	// If the sumRangeMin/sumRangeMax were out of range before, they might be valid now.
	reviewState();
	emitSizeChanged();
}


void REIXSXESImageInterpolationAB::onInputSourceStateChanged()
{
	onInputSourceSizeChanged();	// just in case the size has changed, while it was invalid.
	reviewState();
}

void REIXSXESImageInterpolationAB::setCurve2Disabled(bool disabled)
{
	if(disabled == curve2Disabled_)
		return;

	curve2Disabled_ = disabled;

	if(inputSource_)
	{
		computeCachedValues();
		setModified(true);
		emitValuesChanged();
	}

}


void REIXSXESImageInterpolationAB::setCorrelation1CenterPixel(int centerPx)
{
	if(centerPx == correlation1CenterPx_)
		return;

	correlation1CenterPx_ = centerPx;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::setCorrelation1HalfWidth(int width)
{
	if(correlation1HalfWidth_ == width)
		return;

	correlation1HalfWidth_ = width;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::setCorrelation2CenterPixel(int centerPx)
{
	if(centerPx == correlation2CenterPx_)
		return;

	correlation2CenterPx_ = centerPx;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::setCorrelation2HalfWidth(int width)
{
	if(correlation2HalfWidth_ == width)
		return;
	correlation2HalfWidth_ = width;
	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::enableLiveCorrelation(bool enabled)
{
	if(enabled == liveCorrelation_)
		return;

	liveCorrelation_ = enabled;
	if(liveCorrelation_)
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::correlateNow()
{


	if(!inputSource_ || !inputSource_->isValid())
		return;


	// variables:
	int sizeY = inputSource_->size(1);
	int sizeX = inputSource_->size(0);
	int midY = (sumRangeMinY()+sumRangeMaxY())/2;

	if(sizeY < 1 || sizeX < 1) {
		AMErrorMon::alert(this, -37, "Image correlation routine should not be run with a size of 0. Please report this bug to the REIXS Acquaman developers.");
		return;
	}


	// grab the whole input image for fast access. This is much faster than calling inputSource_->value() repeatedly.
	QVector<double> image(sizeX*sizeY);
	if(!inputSource_->values(AMnDIndex(0,0), AMnDIndex(sizeX-1, sizeY-1), image.data())) {
		AMErrorMon::report(this, AMErrorReport::Alert, -333, "Could not retrieve values from detector image. Please report this problem to the REIXS Acquaman developers.");
		return;
	}


	////////Correlate shift1

	int minShift = -correlation1HalfWidth_;
	int maxShift = correlation1HalfWidth_;


	// initialize all shifts to 0.
	QVector<int> shifts1 = QVector<int>(sizeY);

	// Loop through all the rows (j), but skip the middle one. Find the best shift at each row.
	for(int j=0; j<sizeY; j++) {
		if(j == midY) {
			shifts1[j] = 0;
			continue;
		}

		double bestSum = 0;
		int bestShift = 0;
		for(int shift = minShift; shift<=maxShift; shift++) {	// try out all possible shift values
			// compute the correlation with the center row at this shift
			double correlationSum = 0;
			for(int i = correlation1CenterPx_ - correlation1HalfWidth_, ic = correlation1CenterPx_+correlation1HalfWidth_; i<=ic; i++)
				if(i+shift<sizeX && i+shift>=0 && i<sizeX && i>=0)
					correlationSum += image.at(i*sizeY + midY) * image.at((i+shift)*sizeY + j);
			if(correlationSum > bestSum) {
				bestSum = correlationSum;
				bestShift = shift;
			}
		}
		// now we know the shift for this row
		shifts1[j] = bestShift;
	}


	if(correlation1Smoothing_.first != 0) { //0 is no smoothing


		// Apply smoothing to reduce the noise:
		QVector<double> weights(sizeY, 0.0);
		weights[sumRangeMinY_] = 0.5;
		weights[sumRangeMaxY_] = 0.5;
		for(int j=sumRangeMinY_+1; j<sumRangeMaxY_; ++j)
			weights[j] = 1.0;


		if(curve1Smoother_) shifts1 = curve1Smoother_->smooth(shifts1, weights);


		// Ensure the center-row shift is 0. [Not sure why it ends up being non-zero]. Slide everything so it is.
		int centerShift = shifts1[midY];
		for(int j=0; j<sizeY; ++j)
			shifts1[j] -= centerShift;
	}

	// use the new shift values.
	setShiftValues1(shifts1.toList());



	////////Correlate shift2

	minShift = -correlation2HalfWidth_;
	maxShift = correlation2HalfWidth_;


	// initialize all shifts to 0.
	QVector<int> shifts2 = QVector<int>(sizeY);

	// Loop through all the rows (j), but skip the middle one. Find the best shift at each row.
	for(int j=0; j<sizeY; j++) {
		if(j == midY) {
			shifts2[j] = 0;
			continue;
		}

		double bestSum = 0;
		int bestShift = 0;
		for(int shift = minShift; shift<=maxShift; shift++) {	// try out all possible shift values
			// compute the correlation with the center row at this shift
			double correlationSum = 0;
			for(int i = correlation2CenterPx_ - correlation2HalfWidth_, ic = correlation2CenterPx_+correlation2HalfWidth_; i<=ic; i++)
				if(i+shift<sizeX && i+shift>=0 && i<sizeX && i>=0)
					correlationSum += image.at(i*sizeY + midY) * image.at((i+shift)*sizeY + j);
			if(correlationSum > bestSum) {
				bestSum = correlationSum;
				bestShift = shift;
			}
		}
		// now we know the shift for this row
		shifts2[j] = bestShift;
	}


	if(correlation2Smoothing_.first != 0) { //0 is no smoothing


		// Apply smoothing to reduce the noise:
		QVector<double> weights(sizeY, 0.0);
		weights[sumRangeMinY_] = 0.5;
		weights[sumRangeMaxY_] = 0.5;
		for(int j=sumRangeMinY_+1; j<sumRangeMaxY_; ++j)
			weights[j] = 1.0;


		if(curve2Smoother_) shifts2 = curve2Smoother_->smooth(shifts2, weights);


		// Ensure the center-row shift is 0. [Not sure why it ends up being non-zero]. Slide everything so it is.
		int centerShift = shifts2[midY];
		for(int j=0; j<sizeY; ++j)
			shifts2[j] -= centerShift;
	}

	// use the new shift values.
	setShiftValues2(shifts2.toList());




}



#include "analysis/REIXS/REIXSXESImageInterpolationABEditor.h"

QWidget * REIXSXESImageInterpolationAB::createEditorWidget()
{
	return new REIXSXESImageInterpolationABEditor(this);
}

#include "dataman/AMScan.h"
#include "dataman/REIXS/REIXSXESCalibration2.h"
#include "dataman/AMScan.h"

void REIXSXESImageInterpolationAB::computeCachedAxisValues() const
{
	axisValueCacheInvalid_ = false;
	axisValuesInvalid_ = true;

	// Accessing spectrometer positions: get from scan's scanInitialConditions
	if(scan() == 0) {
		AMErrorMon::alert(this, -1, "Could not calculate energy axis: could not access the XES scan.");
		return;
	}
	const AMControlInfoList* positions = scan()->scanInitialConditions();
	if(!positions) {
		AMErrorMon::alert(this, -1, "Could not calculate energy axis: could not access the spectrometer positions at the start of the scan.");
		return;
	}

	// Grab position variables:
	if(positions->indexOf("spectrometerGrating") == -1 ||
			positions->indexOf("spectrometerRotationDrive") == -1 ||
			positions->indexOf("detectorTranslation") == -1 ||
			positions->indexOf("detectorTiltDrive") == -1) {
		AMErrorMon::alert(this, -1, "Could not calculate energy axis: missing some required spectrometer positions at the start of the scan.");
		return;
	}

	int grating = int(positions->controlNamed("spectrometerGrating").value());
	double liftHeight = positions->controlNamed("spectrometerRotationDrive").value();
	double rPrime = positions->controlNamed("detectorTranslation").value();
	double tiltStage = positions->controlNamed("detectorTiltDrive").value();

	// accessing the calibration. For dataman, what version to use? Ok to be compiled in? Need ability to have separate cal for each scan?
	REIXSXESCalibration2 cal;
	if(grating >= cal.gratingCount()) {
		AMErrorMon::alert(this, -1, "Could not calculate energy axis: the scan's grating was not valid.");
		return;
	}


	double mmPerPixel = cal.detectorWidth() / size(0);
	double grooveDensity = cal.gratingAt(grating).grooveDensity();
	double sinAlpha = sin(cal.d2r(cal.gratingAt(grating).alpha()));

	// beta is also the nominal incidence angle onto the detector. Get beta from spectrometer position:
	double beta = cal.betaFromPosition(grating, cal.spectrometerAngle(liftHeight), rPrime);
	// Any detector tilt offset from nominal: get from position of tilt stage:
	double tiltOffset = cal.tiltOffset(tiltStage, beta);

	// Variables. From here on, we work in radians, instead of the degree convention used by REIXSXESCalibration.
	beta = cal.d2r(beta);
	tiltOffset = cal.d2r(tiltOffset + tiltCalibrationOffset_);	// NOTE: we're adding in the user-specified correction tilt here.

	double sinBeta = sin(beta);
	double cosBeta = cos(beta);

	// Mark's Implementation: This algorithm taken from SXEDAQ and the old ALS BL8 software.
	//////////////////////////////////////////////////////

	// Gamma is the tilt offset:
	double gamma = tiltOffset;

	// Gamma' (gp) is the angle between the central ray and the detector surface, it is acute for higher-than-center energies, and obtuse for lower-than-center energies.
	// higher-than-center energies: gp = pi/2 - beta + tiltOffset
	// lower-than-center energies: gp = pi/2 + beta - tiltOffset.

	// "sign" will be defined inside the loop to be '1' for higher energies (pixel i > 512), and '-1' for lower energies (pixel i < 512).
	// We calculate sin(gp) and cos(gp) just once, recognizing that we'll need to multiply by sign once we get into the pixel-loop.

	/* gamma' (gp) = pi/2 +(gamma-beta)*sign...so...

		sin(gp) = sin(pi/2 + (gamma-beta)*sign)
			 = cos( (gamma-beta)*sign )
			 = cos(gamma-beta)
			 = [ cosb*cos( gamma ) + sinb*sin( gamma ) ]
		cos(gp) = cos(pi/2 + (gamma-beta)*sign)
		  = -sin( (gamma-beta)*sign )
		  = -sign* sin(gamma-beta)
		  = sign* sin(beta - gamma)
		  = sign* sinb*cos( gamma ) - sin( gamma )*cosb	// Note: we'll fill in the sign inside the loop.
	*/
	double singp = cosBeta*cos(gamma) + sinBeta*sin(gamma);
	double cosgp = sinBeta*cos(gamma) - cosBeta*sin(gamma);

	// Calculate bottom half of the axis. (low energies). Sign is -1
	int sign = -1;
	int centerPixel = size(0)/2;

	for(int i=0; i<centerPixel; ++i) {
		// distance away from center, always positive.
		double dx = (centerPixel-i)*mmPerPixel;
		// sindb: sin("delta Beta"): the angle difference from the nominal beta.
		double sindb = sign*( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*sign) );	//you can derive this from sinA/a=sinB/b and c^2=a^2+b^2-2ab*cosC
		//bp ("beta-prime") is the diffraction angle at detector point 'i', sinbp = sin( beta + db )
		//																		 = sinb*cos(db) + cosb*sindb
		//																		 = sinb*sqrt(1-sin^2(db)) + cosb*sindb
		double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
		//solving the grating equation for eV:
		cachedAxisValues_[i] = 0.0012398417*grooveDensity / (sinAlpha - sinbp) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.
	}
	qDebug() << cachedAxisValues_.size();
	// midpoint:
	cachedAxisValues_[centerPixel] = 0.0012398417*grooveDensity / (sinAlpha - sinBeta) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.

	// Calculate top half of axis. (high energies). Sign is 1:
	sign = 1;
	for(int i=centerPixel+1, cc=size(0); i<cc; ++i) {
		// distance away from center, always positive.
		double dx = (i-centerPixel)*mmPerPixel;
		double sindb = sign*( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*sign) );
		double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
		cachedAxisValues_[i] = 0.0012398417*grooveDensity / (sinAlpha - sinbp) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.
	}

	axisValuesInvalid_ = false;
}

bool REIXSXESImageInterpolationAB::isWithinMaskEllipse(double xVal, double yVal, double width, double height) const
{
	return ((fabs(xVal) <= width/2.0*(1.0 - rangeRound_)) || (fabs(yVal) <= height/2.0*(1.0 - rangeRound_)) || ((((xVal-(1-rangeRound_)*width/2.0)/(rangeRound_*width/2.0))*((xVal-(1-rangeRound_)*width/2.0)/(rangeRound_*width/2.0))+((yVal-(1-rangeRound_)*height/2.0)/(rangeRound_*height/2.0))*((yVal-(1-rangeRound_)*height/2.0)/(rangeRound_*height/2.0))) < 1));
}

void REIXSXESImageInterpolationAB::setEnergyCalibrationOffset(double energyCalibrationOffset)
{
	if(energyCalibrationOffset == energyCalibrationOffset_)
		return;

	energyCalibrationOffset_ = energyCalibrationOffset;
	axisValueCacheInvalid_ = true;
	emitSizeChanged();
	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setTiltCalibrationOffset(double tiltCalibrationOffset)
{
	if(tiltCalibrationOffset == tiltCalibrationOffset_)
		return;

	tiltCalibrationOffset_ = tiltCalibrationOffset;
	axisValueCacheInvalid_ = true;
	emitSizeChanged();
	emitValuesChanged();
	setModified(true);
}


void REIXSXESImageInterpolationAB::setCorrelation1SmoothingType(int type)
{
	correlation1Smoothing_.first = type;

	emitValuesChanged();

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelation1SmoothingMode(int mode)
{
	correlation1Smoothing_.second = mode;

	emitValuesChanged();

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelation1Smoothing(QPair<int,int> cSmooth)
{
	int type = cSmooth.first;
	int mode = cSmooth.second;

	if(cSmooth == correlation1Smoothing_)
		return;

	correlation1Smoothing_ = cSmooth;

	delete curve1Smoother_;
	switch((ShiftCurveSmoothing)type) {
	case Poly:
	{
		if(mode == 2)
			curve1Smoother_ = new REIXSQuadraticFitter();
		else if(mode == 3)
			curve1Smoother_ = new REIXSCubicFitter();
		else
			curve1Smoother_ = new REIXSQuarticFitter();
		break;
	}
	case Median:
		curve1Smoother_ = new REIXSMovingMedianFitter(mode);
		break;
	case Average:
		curve1Smoother_ = new REIXSMovingAverageFitter(mode);
		break;
	case None:
	default:
		curve1Smoother_ = 0;
		break;
	}

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}





void REIXSXESImageInterpolationAB::setCorrelation2SmoothingType(int type)
{
	correlation2Smoothing_.first = type;

	emitValuesChanged();

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelation2SmoothingMode(int mode)
{
	correlation2Smoothing_.second = mode;

	emitValuesChanged();

	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelation2Smoothing(QPair<int,int> cSmooth)
{
	int type = cSmooth.first;
	int mode = cSmooth.second;

	if(cSmooth == correlation2Smoothing_)
		return;

	correlation2Smoothing_ = cSmooth;

	delete curve2Smoother_;
	switch((ShiftCurveSmoothing)type) {
	case Poly:
	{
		if(mode == 2)
			curve2Smoother_ = new REIXSQuadraticFitter();
		else if(mode == 3)
			curve2Smoother_ = new REIXSCubicFitter();
		else
			curve2Smoother_ = new REIXSQuarticFitter();
		break;
	}
	case Median:
		curve2Smoother_ = new REIXSMovingMedianFitter(mode);
		break;
	case Average:
		curve2Smoother_ = new REIXSMovingAverageFitter(mode);
		break;
	case None:
	default:
		curve2Smoother_ = 0;
		break;
	}


	if(liveCorrelation())
	{
		callCorrelation_.schedule();
	}

	setModified(true);
}

void REIXSXESImageInterpolationAB::setBinningLevel(int binningLevel)
{
	binningLevel_ = binningLevel;
	setModified(true);
	if (inputSource_)
		onInputSourceSizeChanged();
}
