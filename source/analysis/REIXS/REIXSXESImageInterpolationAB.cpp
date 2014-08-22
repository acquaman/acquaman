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

REIXSXESImageInterpolationAB::REIXSXESImageInterpolationAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{

	curveSmoother_ = 0;

	sumRangeMinY_ = 10;
	sumRangeMaxY_ = 53;
	sumRangeMinX_ = 75;
	sumRangeMaxX_ = 925;
	rangeRound_ = 0.6;
	correlationCenterPx_ = 512;
	correlationHalfWidth_ = 40;
	correlationSmoothing_ = QPair<int,int>(-1,1);  //-1,1???
	liveCorrelation_ = false;
	setCorrelationSmoothing(QPair<int,int>(1,2)); //poly,quadratic

	energyCalibrationOffset_ = 0;
	tiltCalibrationOffset_ = 0;

	// Live correlation turned on by default. Need to make sure that this is OK for performance; it should be now that we're using block access.
	liveCorrelation_ = false;
	// shift values can start out empty.

	inputSource_ = 0;
	cacheInvalid_ = true;
	axisValueCacheInvalid_ = true;
	axisValuesInvalid_ = false;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	//connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
	setDescription("XES Interpolated Spectrum");

	// FOR TESTING
	interpolationLevel_ = 10;
	shiftPosition1_ = 250;
	shiftPosition2_ = 700;
	shiftValues1_ << 0 << 0 << 0 << 0 << 3 << 6 << 9 << 11 << 12 << 13 << 15 << 16 << 17 << 14 << 12 << 10 << 9 << 8 << 8 << 7 << 7 << 6 << 6 << 5 << 4 << 4 << 3 << 2 << 2 << 1 << 0 << 0 << -1 << -2 << -2 << -3 << -4 << -5 << -6 << -7 << -8 << -9 << -10 << -12 << -13 << -13 << -14 << -14 << -11 << -7 << -2 << 2 << 7 << 9 << 11 << 12 << 13 << 12 << 9 << 6 << 4 << 0 << 0 << 0;
	shiftValues2_ << 0 << 0 << 0 << -4 << -6 << -8 << -10 << -11 << -12 << -13 << -13 << -14 << -11 << -9 << -7 << -6 << -5 << -5 << -4 << -3 << -3 << -2 << -2 << -1 << -1 << -1 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << -1 << -1 << -2 << -3 << -4 << -6 << -9 << -12 << -12 << -12 << -11 << -11 << -10 << -8 << -6 << -4 << 0 << 0 << 0;
	//shiftValues2_.clear();
}

REIXSXESImageInterpolationAB::REIXSXESImageInterpolationAB(AMDatabase *db, int id) :
	AMStandardAnalysisBlock("tempName", 0)
{
	curveSmoother_ = 0;

	sumRangeMinY_ = 3;
	sumRangeMaxY_ = 60;
	sumRangeMinX_ = 50;
	sumRangeMaxX_ = 950;
	rangeRound_ = 1.0;
	correlationCenterPx_ = 512;
	correlationHalfWidth_ = 40;
	correlationSmoothing_ = QPair<int,int>(-1,1);
	energyCalibrationOffset_ = 0;
	tiltCalibrationOffset_ = 0;
	liveCorrelation_ = false;
	// shift values can start out empty.

	inputSource_ = 0;
	cacheInvalid_ = true;
	axisValueCacheInvalid_ = true;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	loadFromDb(db, id); // will restore the parameters sumRangeMin_, sumRangeMax_, correlation settings, and shift values. We'll remain invalid until we get connected to a data source.
	AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.

	connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
}

REIXSXESImageInterpolationAB::~REIXSXESImageInterpolationAB() {
}

void REIXSXESImageInterpolationAB::setSumRangeMinY(int sumRangeMinY)
{
	// no change
	if(sumRangeMinY == sumRangeMinY_)
		return;

	sumRangeMinY_ = sumRangeMinY;
	if(liveCorrelation())
		callCorrelation_.schedule();

	cacheInvalid_ = true;
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
		callCorrelation_.schedule();

	cacheInvalid_ = true;
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
		callCorrelation_.schedule();

	cacheInvalid_ = true;
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
		callCorrelation_.schedule();

	cacheInvalid_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}



void REIXSXESImageInterpolationAB::setShiftValues1(const AMIntList &shiftValues1)
{
	if(shiftValues1 == shiftValues1_)
		return;

	shiftValues1_ = shiftValues1;
	cacheInvalid_ = true;	// could change all our cached values
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
	cacheInvalid_ = true;	// could change all our cached values
	reviewState();	// might change the state to valid, if we didn't have proper number of shift values before.

	emitValuesChanged();
	emit shiftValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setShiftPosition1(const int& shiftPosition1)
{
	if(shiftPosition1 == shiftPosition1_)
		return;

	shiftPosition1_ = shiftPosition1;
	cacheInvalid_ = true;	// could change all our cached values
	reviewState();	// might change the state to valid, if we didn't have proper number of shift values before.

	emitValuesChanged();
	emit shiftValuesChanged();
	setModified(true);
}

void REIXSXESImageInterpolationAB::setShiftPosition2(const int& shiftPosition2)
{
	if(shiftPosition2 == shiftPosition2_)
		return;

	shiftPosition2_ = shiftPosition2;
	cacheInvalid_ = true;	// could change all our cached values
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
		callCorrelation_.schedule();

	cacheInvalid_ = true;
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
		return true;// always acceptable; the null input.

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

		if(shiftValues_.isEmpty()) {
			for(int i=0,cc=inputSource_->size(1); i<cc; i++)
				shiftValues_ << 0;	// make a null shift list, matching the size of the input data. Don't do this if we already have a shift list.
		}
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

	cacheInvalid_ = true;
	cachedValues_ = QVector<double>(axes_.at(0).size);
	axisValueCacheInvalid_ = true;
	cachedAxisValues_ = QVector<double>(axes_.at(0).size);
	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
}


AMNumber REIXSXESImageInterpolationAB::value(const AMnDIndex &indexes) const
{
	if((indexes.rank() != 1))
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);


#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(((unsigned long)indexes.i() >= (unsigned long)axes_.at(0).size))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if(cacheInvalid_)
		computeCachedValues();

	return AMNumber(cachedValues_.at(indexes.i()));
}

bool REIXSXESImageInterpolationAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;	// dimension error

	if(!isValid())
		return false; // invalid error

	// Max x pixel value:
	int maxI = inputSource_->size(0);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(indexEnd.i() < indexStart.i())
		return false;
	if(indexEnd.i() >= maxI)
		return false;
#endif


	if(cacheInvalid_)
		computeCachedValues();

	memcpy(outputValues, (cachedValues_.constData()+indexStart.i()), (indexEnd.i()-indexStart.i()+1)*sizeof(double));

	return true;
}


void REIXSXESImageInterpolationAB::computeCachedValues() const
{
	int maxI = inputSource_->size(0); //1024
	int maxJ = inputSource_->size(1); //64

	// grab the full image
	QVector<double> image(maxI*maxJ);
	if(!inputSource_->values(AMnDIndex(0,0), AMnDIndex(maxI-1, maxJ-1), image.data())) {
		AMErrorMon::report(this, AMErrorReport::Alert, -333, "Could not retrieve values from detector image. Please report this problem to the REIXS Acquaman developers.");
		cacheInvalid_ = false;	// avoid repeating this error message for every single data point...
		return;
	}

	//If there is on one shift value, don't interpolate.
	if(shiftValues2().isEmpty())
	{
		//The center point of the sum region
		double originX = (double)sumRangeMinX_ + ((double)sumRangeMaxX_ - (double)sumRangeMinX_)/2.0;
		double originY = (double)sumRangeMinY_ + ((double)sumRangeMaxY_ - (double)sumRangeMinY_)/2.0;
		//Width and height of the sum region, in pixels
		double numX = (double)(sumRangeMaxX_ - sumRangeMinX_);
		double numY = (double)(sumRangeMaxY_ - sumRangeMinY_);


		for(int i=0; i<maxI; ++i) {
			double newVal = 0.0;
			int contributingRows = 0;
			if(i > sumRangeMinX_ && i < sumRangeMaxX_) {
				double xVal = (double)i - originX;
				for(int j=sumRangeMinY_; j<=sumRangeMaxY_; j++) { // loop through rows
					if(rangeRound_ == 0.0) { //not ellipse
						int sourceI = i + shiftValues1_.at(j);
						if(sourceI < maxI && sourceI >= 0) {
							newVal += image.at(sourceI*maxJ + j);
							contributingRows++;
						}
					}
					else {

						double yVal = (double)j - originY;
						if((fabs(xVal) <= numX/2.0*(1.0 - rangeRound_)) || (fabs(yVal) <= numY/2.0*(1.0 - rangeRound_)) || ((((xVal-(1-rangeRound_)*numX/2.0)/(rangeRound_*numX/2.0))*((xVal-(1-rangeRound_)*numX/2.0)/(rangeRound_*numX/2.0))+((yVal-(1-rangeRound_)*numY/2.0)/(rangeRound_*numY/2.0))*((yVal-(1-rangeRound_)*numY/2.0)/(rangeRound_*numY/2.0))) < 1)) { //within ellipse
							int sourceI = i + shiftValues1_.at(j);
							if(sourceI < maxI && sourceI >= 0) {
								newVal += image.at(sourceI*maxJ + j);
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

			cachedValues_[i] = newVal;
		}
	}
	else
	{
		computeShiftMap();

		//interpolate image in x, probably rotating image by 90 degree in memory because of a switch in conventions...
		QList< QList<double> > interpolatedImage;
		for(int i = 0; i < 64; i++)
		{
			QList<double> newRow;
			for(int j = 0; j< 1023; j++)
			{
				for(int k = 0; k < interpolationLevel_; k++)
				{
					newRow.append(image[j*maxJ + i] + ((double)k / (double)interpolationLevel_) * (image[(j+1)*maxJ + i] - image[j*maxJ + i]));
				}
			}
			interpolatedImage.append(newRow);
		}

		//OKAY!  Now I think we have a shiftMap_ and a interpolatedImage that mirror those used in Robert's code. Even if it doesn't make any sense, I should be able to copy and paste...
		//***************************************************************************
		//***************************************************************************
		//Stolen right from Robert's Code

		//now shift and integrate into corresponding pixels of original 32x1024 size
		//create a zero-filled list array, same size as shiftMap and interpolatedImage
		QList< QList <double> > finalLargeImage = interpolatedImage;
		for(int i = 0; i < finalLargeImage.count(); i++)
		{
			for(int j = 0 ; j < finalLargeImage[i].count(); j++)
			{
			 finalLargeImage[i][j] = 0;
			}
		}


		//iterate through shiftMap, adding the corresponding element from interpolated to the shifted location in this new array
		for(int i = 0; i < shiftMap_.count(); i++)
		{
			for(int j = 0; j < shiftMap_[i].count(); j++)
			{
				if(((j - (int)(shiftMap_[i][j]*interpolationLevel_)) < shiftMap_[i].count()) && ((j - (int)(shiftMap_[i][j]*interpolationLevel_)) > 0))
					finalLargeImage[i][(j - (int)(shiftMap_[i][j]*interpolationLevel_))] += interpolatedImage[i][j];
			}
		}

		//integrate out this list array
		QList<QList <double> > tempFinalArray;
		for(int i = 0; i < finalLargeImage.count(); i++)
		{
			double integral = 0;
			QList<double> finalRow;
			for(int j = 0; j < finalLargeImage[i].count(); j++)
			{
				integral += finalLargeImage[i][j];
				if(((j % interpolationLevel_) == 0) || (j == 0) || (j == finalLargeImage[i].count() - 1))
				{
					finalRow.append(integral);
					integral = 0;
				}

			}
			tempFinalArray.append(finalRow);
		}

				//normalize by x interpolation factor since we've integrated...
		double normValue = (double)interpolationLevel_;
		if(normValue < 2)
			normValue = 2.0;
//		normValue *= 0.1;  // Um...  Robert, why are you multiplying the spectra by 10?
		for(int i = 0; i < tempFinalArray.count(); i++)
			for(int j = 0; j < tempFinalArray[i].count(); j++)
				tempFinalArray[i][j] /= (normValue);




		//output matrix has to be global so it can be saved to file.
//        for(int i = 0; i < tempFinalArray.count(); i++)
//        {
//            QList<int> tempIntList;
//            for(int j = 0; j < tempFinalArray[i].count(); j++)
//            {
//                tempIntList.append((int)(tempFinalArray[i][j] + 0.5)); //0.5 to ensure proper rounding, rather than truncating
//            }
//            globalFinalImage.append(tempIntList);
//        }

//		***************************************************************************
//		***************************************************************************

//		This neglects the mask:
//				for(int j = 0; j < tempFinalArray[1].count(); j++)
//		{
//			QList<int> tempIntList;
//			for(int i = 0; i < tempFinalArray.count(); i++)
//			{
//				cachedValues_[j] += ((int)(tempFinalArray[i][j] + 0.5)); //0.5 to ensure proper rounding, rather than truncating
//			}
//		}
		//The center point of the sum region
		double originX = (double)sumRangeMinX_ + ((double)sumRangeMaxX_ - (double)sumRangeMinX_)/2.0;
		double originY = (double)sumRangeMinY_ + ((double)sumRangeMaxY_ - (double)sumRangeMinY_)/2.0;
		//Width and height of the sum region, in pixels
		double numX = (double)(sumRangeMaxX_ - sumRangeMinX_);
		double numY = (double)(sumRangeMaxY_ - sumRangeMinY_);


		for(int i=0; i<maxI; ++i) {
			double newVal = 0.0;
			int contributingRows = 0;
			if(i > sumRangeMinX_ && i < sumRangeMaxX_) {
				double xVal = (double)i - originX;
				for(int j=sumRangeMinY_; j<=sumRangeMaxY_; j++) { // loop through rows
					if(rangeRound_ == 0.0) { //not ellipse
							newVal += ((int)(tempFinalArray[j][i] + 0.5)); //0.5 to ensure proper rounding, rather than truncating;
							contributingRows++;
					}
					else {

						double yVal = (double)j - originY;
						if((fabs(xVal) <= numX/2.0*(1.0 - rangeRound_)) || (fabs(yVal) <= numY/2.0*(1.0 - rangeRound_)) || ((((xVal-(1-rangeRound_)*numX/2.0)/(rangeRound_*numX/2.0))*((xVal-(1-rangeRound_)*numX/2.0)/(rangeRound_*numX/2.0))+((yVal-(1-rangeRound_)*numY/2.0)/(rangeRound_*numY/2.0))*((yVal-(1-rangeRound_)*numY/2.0)/(rangeRound_*numY/2.0))) < 1)) { //within ellipse
								newVal += ((int)(tempFinalArray[j][i] + 0.5)); //0.5 to ensure proper rounding, rather than truncating
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

			cachedValues_[i] = newVal;
		}

	}

	cacheInvalid_ = false;
}

void REIXSXESImageInterpolationAB::computeShiftMap() const
{
	QList<QList <double> > smallShiftMap;
	//make a 64x1024 temp shiftMap prior to interpolation
	for(int i = 0; i < 64; i++)
	{
		QList<double> newRow;
		for(int j=0; j< 1024; j++)
		{
			newRow.append( ( (shiftValues1_[i] - shiftValues2_[i]) / ((double)(shiftPosition1_ - shiftPosition2_) ) ) *((double)(j - shiftPosition1_)) + shiftValues1()[i]);
		}
		smallShiftMap.append(newRow);
	}

	//interpolate the shift map
	shiftMap_.clear();
	for(int i = 0; i < 64; i++)
	{
		QList<double> newRow;
		for(int j = 0; j < 1023; j++)
		{
			for(int k = 0; k < interpolationLevel_; k++)
			{
				newRow.append(smallShiftMap[i][j] + ((double) k / (double)interpolationLevel_)*(smallShiftMap[i][j+1] - smallShiftMap[i][j]));
			}
		}
		shiftMap_.append(newRow);
	}
}







AMNumber REIXSXESImageInterpolationAB::axisValue(int axisNumber, int index) const
{
	if((axisNumber != 0))
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(((unsigned)index >= (unsigned)axes_.at(0).size))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if(axisValueCacheInvalid_)
		computeCachedAxisValues();

	if(axisValuesInvalid_)
		return index;

	return cachedAxisValues_.at(index);
}

void REIXSXESImageInterpolationAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	if(liveCorrelation())
		callCorrelation_.schedule();

	// invalidate the cache.
	cacheInvalid_ = true;
	emitValuesChanged();
}

void REIXSXESImageInterpolationAB::onInputSourceSizeChanged()
{
	cacheInvalid_ = true;
	axisValueCacheInvalid_ = true;

	bool sizeChanged = false;
	if(axes_.at(0).size != inputSource_->size(0)) {
		axes_[0].size = inputSource_->size(0);
		cachedValues_.resize(axes_.at(0).size);
		cachedAxisValues_.resize(axes_.at(0).size);
		sizeChanged = true;
	}

	if(liveCorrelation())
		callCorrelation_.schedule();

	// If the sumRangeMin/sumRangeMax were out of range before, they might be valid now.
	reviewState();
	if(sizeChanged)
		emitSizeChanged(0);
	emitValuesChanged();
}


void REIXSXESImageInterpolationAB::onInputSourceStateChanged()
{
	onInputSourceSizeChanged();	// just in case the size has changed, while it was invalid.
	reviewState();
}



void REIXSXESImageInterpolationAB::setCorrelationCenterPixel(int centerPx)
{
	if(centerPx == correlationCenterPx_)
		return;

	correlationCenterPx_ = centerPx;
	if(liveCorrelation())
		callCorrelation_.schedule();

	setModified(true);
}

void REIXSXESImageInterpolationAB::setCorrelationHalfWidth(int width)
{
	if(correlationHalfWidth_ == width)
		return;

	correlationHalfWidth_ = width;
	if(liveCorrelation())
		callCorrelation_.schedule();

	setModified(true);
}

void REIXSXESImageInterpolationAB::enableLiveCorrelation(bool enabled)
{
	if(enabled == liveCorrelation_)
		return;

	liveCorrelation_ = enabled;
	if(liveCorrelation_)
		callCorrelation_.schedule();

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

	int minShift = -correlationHalfWidth_;
	int maxShift = correlationHalfWidth_;

	// grab the whole input image for fast access. This is much faster than calling inputSource_->value() repeatedly.
	QVector<double> image(sizeX*sizeY);
	if(!inputSource_->values(AMnDIndex(0,0), AMnDIndex(sizeX-1, sizeY-1), image.data())) {
		AMErrorMon::report(this, AMErrorReport::Alert, -333, "Could not retrieve values from detector image. Please report this problem to the REIXS Acquaman developers.");
		return;
	}



	// initialize all shifts to 0.
	QVector<int> shifts = QVector<int>(sizeY);

	// Loop through all the rows (j), but skip the middle one. Find the best shift at each row.
	for(int j=0; j<sizeY; j++) {
		if(j == midY) {
			shifts[j] = 0;
			continue;
		}

		double bestSum = 0;
		int bestShift = 0;
		for(int shift = minShift; shift<=maxShift; shift++) {	// try out all possible shift values
			// compute the correlation with the center row at this shift
			double correlationSum = 0;
			for(int i = correlationCenterPx_ - correlationHalfWidth_, ic = correlationCenterPx_+correlationHalfWidth_; i<=ic; i++)
				if(i+shift<sizeX && i+shift>=0 && i<sizeX && i>=0)
					correlationSum += image.at(i*sizeY + midY) * image.at((i+shift)*sizeY + j);
			if(correlationSum > bestSum) {
				bestSum = correlationSum;
				bestShift = shift;
			}
		}
		// now we know the shift for this row
		shifts[j] = bestShift;
	}


	if(correlationSmoothing_.first != 0) { //0 is no smoothing


		// Apply smoothing to reduce the noise:
		QVector<double> weights(sizeY, 0.0);
		weights[sumRangeMinY_] = 0.5;
		weights[sumRangeMaxY_] = 0.5;
		for(int j=sumRangeMinY_+1; j<sumRangeMaxY_; ++j)
			weights[j] = 1.0;


		if(curveSmoother_) shifts = curveSmoother_->smooth(shifts, weights);


		// Ensure the center-row shift is 0. [Not sure why it ends up being non-zero]. Slide everything so it is.
		int centerShift = shifts[midY];
		for(int j=0; j<sizeY; ++j)
			shifts[j] -= centerShift;
	}

	// use the new shift values.
	setShiftValues1(shifts.toList());
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

	// Gamma' (gp) is the angle between the central ray and the detector surface; it is acute for higher-than-center energies, and obtuse for lower-than-center energies.
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
		// distance away from center; always positive.
		double dx = (centerPixel-i)*mmPerPixel;
		// sindb: sin("delta Beta"): the angle difference from the nominal beta.
		double sindb = sign*( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*sign) );	//you can derive this from sinA/a=sinB/b and c^2=a^2+b^2-2ab*cosC
		//bp ("beta-prime") is the diffraction angle at detector point 'i'; sinbp = sin( beta + db )
		//																		 = sinb*cos(db) + cosb*sindb
		//																		 = sinb*sqrt(1-sin^2(db)) + cosb*sindb
		double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
		//solving the grating equation for eV:
		cachedAxisValues_[i] = 0.0012398417*grooveDensity / (sinAlpha - sinbp) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.
	}

	// midpoint:
	cachedAxisValues_[centerPixel] = 0.0012398417*grooveDensity / (sinAlpha - sinBeta) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.

	// Calculate top half of axis. (high energies). Sign is 1:
	sign = 1;
	for(int i=centerPixel+1, cc=size(0); i<cc; ++i) {
		// distance away from center; always positive.
		double dx = (i-centerPixel)*mmPerPixel;
		double sindb = sign*( dx*singp/sqrt(rPrime*rPrime + dx*dx - 2*rPrime*dx*cosgp*sign) );
		double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
		cachedAxisValues_[i] = 0.0012398417*grooveDensity / (sinAlpha - sinbp) + energyCalibrationOffset_;	// NOTE: we're adding in the user-specified energy offset here.
//		qDebug()<< "sindb = " << sindb;
//		qDebug()<< "sinbp = " << sinbp;

	}
	//////////////////////////////////////////////////////


	// David's implementation: (SUSPICIOUS?!?)
	//////////////////////////////////////////////////////
//	double singp = cos(beta);
//	double cosgp = sin(beta);

//	int centerPixel = size(0)/2;
//	for(int i=0, cc=size(0); i<cc; ++i) {

//		// distance away from center; always positive.
//		double dx = (centerPixel-i)*mmPerPixel*singp;

//		// db: "delta Beta": the angle difference from the nominal beta.

//		double db = atan(dx/(rPrime-(centerPixel-i)*mmPerPixel*cosgp));


//		//bp ("beta-prime") is the diffraction angle at detector point 'i'; sinbp = sin( beta + db )
//		//																		 = sinb*cos(db) + cosb*sindb
//		//																		 = sinb*sqrt(1-sin^2(db)) + cosb*sindb
//		//double sinbp = sinBeta*sqrt( 1.0-sindb*sindb ) + cosBeta*sindb;
//		double sinbp = sin(beta - db);


//		//solving the grating equation for eV:
//		cachedAxisValues_[i] = 0.0012398417*grooveDensity / (sinAlpha - sinbp);
//	}
	////////////////////////////////////////////////////////

//	qDebug()<< "rPrime = " << rPrime;
//	qDebug() <<"cosgp = " << cosgp;
//	qDebug()<< "cosBeta = " << cosBeta;
//	qDebug()<< "sinBeta = " << sinBeta;
//	qDebug()<< "gamma = " << gamma;
//	qDebug()<< "beta = " << beta;
//	qDebug()<< "grooveDensity = " << grooveDensity;
//	qDebug()<< "sinAlpha = " << sinAlpha;


	axisValuesInvalid_ = false;
}

void REIXSXESImageInterpolationAB::setEnergyCalibrationOffset(double energyCalibrationOffset)
{
	if(energyCalibrationOffset == energyCalibrationOffset_)
		return;

	energyCalibrationOffset_ = energyCalibrationOffset;
	axisValueCacheInvalid_ = true;
	emitValuesChanged();

	setModified(true);
}

void REIXSXESImageInterpolationAB::setTiltCalibrationOffset(double tiltCalibrationOffset)
{
	if(tiltCalibrationOffset == tiltCalibrationOffset_)
		return;

	tiltCalibrationOffset_ = tiltCalibrationOffset;
	axisValueCacheInvalid_ = true;
	emitValuesChanged();

	setModified(true);
}
void REIXSXESImageInterpolationAB::setCorrelationSmoothingType(int type)
{
	correlationSmoothing_.first = type;

	emitValuesChanged();

	if(liveCorrelation())
			callCorrelation_.schedule();

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelationSmoothingMode(int mode)
{
	correlationSmoothing_.second = mode;

	emitValuesChanged();

	if(liveCorrelation())
		callCorrelation_.schedule();

	setModified(true);

}

void REIXSXESImageInterpolationAB::setCorrelationSmoothing(QPair<int,int> cSmooth)
{
	int type = cSmooth.first;
	int mode = cSmooth.second;

	if(cSmooth == correlationSmoothing_)
		return;

	correlationSmoothing_ = cSmooth;

	delete curveSmoother_;
	switch((ShiftCurveSmoothing)type) {
	case Poly:
	{
		if(mode == 2)
			curveSmoother_ = new REIXSQuadraticFitter();
		else if(mode == 3)
			curveSmoother_ = new REIXSCubicFitter();
		else
			curveSmoother_ = new REIXSQuarticFitter();
		break;
	}
	case Median:
		curveSmoother_ = new REIXSMovingMedianFitter(mode);
		break;
	case Average:
		curveSmoother_ = new REIXSMovingAverageFitter(mode);
		break;
	case None:
	default:
		curveSmoother_ = 0;
		break;
	}


	if(liveCorrelation())
			callCorrelation_.schedule();

	setModified(true);
}


