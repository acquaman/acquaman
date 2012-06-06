/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "REIXSXESImageAB.h"

#include "util/AMErrorMonitor.h"

REIXSXESImageAB::REIXSXESImageAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{

	sumRangeMin_ = 5;
	sumRangeMax_ = 58;
	correlationCenterPx_ = 512;
	correlationHalfWidth_ = 40;
	liveCorrelation_ = true;
	// shift values can start out empty.

	inputSource_ = 0;
	cacheInvalid_ = true;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	// Fitting structures (unallocated)
	fitWorkSpace_ = 0;
	fitXX_ = 0;
	fitY_ = 0;
	fitWeight_ = 0;
	fitC_ = 0;
	fitCov_ = 0;

	connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
	setDescription("XES Analyzed Spectrum");
}

REIXSXESImageAB::REIXSXESImageAB(AMDatabase *db, int id) :
	AMStandardAnalysisBlock("tempName", 0)
{
	sumRangeMin_ = 5;
	sumRangeMax_ = 58;
	correlationCenterPx_ = 512;
	correlationHalfWidth_ = 40;
	liveCorrelation_ = false;
	// shift values can start out empty.

	inputSource_ = 0;
	cacheInvalid_ = true;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	// Fitting structures (unallocated)
	fitWorkSpace_ = 0;
	fitXX_ = 0;
	fitY_ = 0;
	fitWeight_ = 0;
	fitC_ = 0;
	fitCov_ = 0;

	loadFromDb(db, id); // will restore the parameters sumRangeMin_, sumRangeMax_, correlation settings, and shift values. We'll remain invalid until we get connected to a data source.
	AMDataSource::name_ = AMDbObject::name();	// normally it's not okay to change a dataSource's name. Here we get away with it because we're within the constructor, and nothing's watching us yet.

	connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
}

REIXSXESImageAB::~REIXSXESImageAB() {
	releaseFittingStructures();
}

void REIXSXESImageAB::setSumRangeMin(int sumRangeMin)
{
	// no change
	if(sumRangeMin == sumRangeMin_)
		return;

	sumRangeMin_ = sumRangeMin;
	if(liveCorrelation())
		callCorrelation_.schedule();

	cacheInvalid_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageAB::setSumRangeMax(int sumRangeMax)
{
	if(sumRangeMax == sumRangeMax_)
		return;

	sumRangeMax_ = sumRangeMax;
	if(liveCorrelation())
		callCorrelation_.schedule();

	cacheInvalid_ = true;
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageAB::setShiftValues(const AMIntList &shiftValues)
{
	if(shiftValues == shiftValues_)
		return;

	shiftValues_ = shiftValues;
	cacheInvalid_ = true;	// could change all our cached values
	reviewState();	// might change the state to valid, if we didn't have proper number of shift values before.

	emitValuesChanged();
	emit shiftValuesChanged();
	setModified(true);
}



void REIXSXESImageAB::reviewState()
{

	if(inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	int s = inputSource_->size(1);

	if(sumRangeMin_ >= s || sumRangeMax_ >= s) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	if(sumRangeMin_ > sumRangeMax_) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	if(shiftValues_.size() != s) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	setState(0);

}


bool REIXSXESImageAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if(dataSources.isEmpty())
		return true;// always acceptable; the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 2)
		return true;

	return false;
}

void REIXSXESImageAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
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

		if(shiftValues_.isEmpty()) {
			for(int i=0,cc=inputSource_->size(1); i<cc; i++)
				shiftValues_ << 0;	// make a null shift list, matching the size of the input data. Don't do this if we already have a shift list.
		}

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	cacheInvalid_ = true;
	cachedValues_ = QVector<double>(axes_.at(0).size);
	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
}


AMNumber REIXSXESImageAB::value(const AMnDIndex &indexes) const
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

bool REIXSXESImageAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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


void REIXSXESImageAB::computeCachedValues() const
{
	int maxI = inputSource_->size(0);
	int maxJ = inputSource_->size(1);

	// grab the full image
	QVector<double> image(maxI*maxJ);
	if(!inputSource_->values(AMnDIndex(0,0), AMnDIndex(maxI-1, maxJ-1), image.data())) {
		AMErrorMon::report(this, AMErrorReport::Alert, -333, "Could not retrieve values from detector image. Please report this problem to the REIXS Acquaman developers.");
		cacheInvalid_ = false;	// avoid repeating this error message for every single data point...
		return;
	}


	for(int i=0; i<maxI; ++i) {
		double newVal = 0.0;
		int contributingRows = 0;
		for(int j=sumRangeMin_; j<=sumRangeMax_; j++) { // loop through rows
			int sourceI = i + shiftValues_.at(j);
			if(sourceI < maxI && sourceI >= 0) {
				newVal += image.at(sourceI*maxJ + j);
				contributingRows++;
			}
		}

		// normalize by dividing by the number of rows that contributed. Since we want to keep the output in units similar to raw counts, multiply by the nominal (usual) number of contributing rows.
		// Essentially, this normalization prevents columns near the edge that miss out on some rows due to shifting from being artificially suppressed.  For inner columns, contributingRows will (sumRangeMax_ - sumRangeMin_ + 1).
		if(contributingRows == 0)
			newVal = 0;
		else
			newVal = newVal * double(sumRangeMax_ - sumRangeMin_ + 1) / double(contributingRows);

		cachedValues_[i] = newVal;
	}
	cacheInvalid_ = false;
}


AMNumber REIXSXESImageAB::axisValue(int axisNumber, int index) const
{
	if((axisNumber != 0))
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(((unsigned)index >= (unsigned)axes_.at(0).size))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	return index;
}

void REIXSXESImageAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)
	
	if(liveCorrelation())
		callCorrelation_.schedule();

	// invalidate the cache.
	cacheInvalid_ = true;
	emitValuesChanged();
}

void REIXSXESImageAB::onInputSourceSizeChanged()
{
	cacheInvalid_ = true;

	bool sizeChanged = false;
	if(axes_.at(0).size != inputSource_->size(0)) {
		axes_[0].size = inputSource_->size(0);
		cachedValues_.resize(axes_.at(0).size);
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


void REIXSXESImageAB::onInputSourceStateChanged()
{
	reviewState();
	onInputSourceSizeChanged();	// just in case the size has changed, while it was invalid.
}



void REIXSXESImageAB::setCorrelationCenterPixel(int centerPx)
{
	if(centerPx == correlationCenterPx_)
		return;

	correlationCenterPx_ = centerPx;
	if(liveCorrelation())
		callCorrelation_.schedule();

	setModified(true);
}

void REIXSXESImageAB::setCorrelationHalfWidth(int width)
{
	if(correlationHalfWidth_ == width)
		return;

	correlationHalfWidth_ = width;
	if(liveCorrelation())
		callCorrelation_.schedule();

	setModified(true);
}

void REIXSXESImageAB::enableLiveCorrelation(bool enabled)
{
	if(enabled == liveCorrelation_)
		return;

	liveCorrelation_ = enabled;
	if(liveCorrelation_)
		callCorrelation_.schedule();

	setModified(true);
}

void REIXSXESImageAB::correlateNow()
{
	if(!inputSource_ || !inputSource_->isValid())
		return;

	// variables:
	int sizeY = inputSource_->size(1);
	int sizeX = inputSource_->size(0);
	int midY = (sumRangeMin()+sumRangeMax())/2;

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
		if(j == midY)
			continue;

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


	// Apply quadratic fit to smooth out the noise:
	shifts = quadraticSmooth(shifts);

	// use the new shift values.
	setShiftValues(shifts.toList());
}

void REIXSXESImageAB::allocateFittingStructures(int numRows)
{
	if(numRows < 1)
		return;	// do not allocate if size is 0.

	fitWorkSpace_ = gsl_multifit_linear_alloc(numRows, 3);
	fitXX_ = gsl_matrix_calloc(numRows, 3);		// The matrix of predictor variables XX_ij = x_i^j;  In our case, x is the image row index (y!)
	fitY_ = gsl_vector_calloc(numRows);		// The vector of observations;  In our case, fitY_i is the shiftnumber shiftnum_[i]
	fitWeight_ = gsl_vector_calloc(numRows);	// The weight of the observation; In our case 1 for sourceRangeMin_ <= y <= sourceRangeMax_; 0 otherwise.
	fitC_ = gsl_vector_calloc(3);		// The vector of coefficients; In our case the 3 quadratic fit constants.
	fitCov_ = gsl_matrix_calloc(3, 3);
	// Fill the predictor matrix...
	for(int j=0; j<numRows; j++) {
		gsl_matrix_set(fitXX_, j, 0, 1);
		gsl_matrix_set(fitXX_, j, 1, j);
		gsl_matrix_set(fitXX_, j, 2, j*j);
	}
}

void REIXSXESImageAB::releaseFittingStructures()
{
	if(fitWorkSpace_) {
		gsl_multifit_linear_free( fitWorkSpace_ );
		gsl_matrix_free( fitXX_ );
		gsl_vector_free( fitY_ );
		gsl_vector_free( fitWeight_ );
		gsl_vector_free( fitC_ );
		gsl_matrix_free( fitCov_ );

		fitWorkSpace_ = 0;
		fitXX_ = 0;
		fitY_ = 0;
		fitWeight_ = 0;
		fitC_ = 0;
		fitCov_ = 0;
	}
}

QVector<int> REIXSXESImageAB::quadraticSmooth(const QVector<int> &shifts)
{
	int sizeY = inputSource_->size(1);
	QVector<int> rv(sizeY);	// initialize return value.

	if(shifts.count() != sizeY) {
		AMErrorMon::alert(this, -33, "Cannot conduct quadratic smoothing of the shift values, because the size of the input does not match the image vertical size. Please report this bug to the REIXS Acquaman developers.");
		return rv;
	}

	// Do we need to allocate or re-size the fitting structures?
	if(!fitWorkSpace_ || fitY_->size != size_t(sizeY)) {
		releaseFittingStructures();
		allocateFittingStructures(sizeY);
	}

	// Copy the shiftnumbers into the 'observation' fitting vector
	// Set the weights to 0 outside [sumRangeMin_, sumRangeMax_] and 1 inside. (0.5 on the boundary, for non-abruptness?)
	for(int j=0; j<sizeY; j++) {
		gsl_vector_set(fitY_, j, shifts.at(j));
		if(j<sumRangeMin_ || j>sumRangeMax_)
			gsl_vector_set(fitWeight_, j, 0);
		else if(j==sumRangeMin_ || j==sumRangeMax_)
			gsl_vector_set(fitWeight_, j, 0.5);
		else
			gsl_vector_set(fitWeight_, j, 1);
	}

	// Run the fitting procedure
	if( gsl_multifit_linear (fitXX_, fitY_, fitC_, fitCov_, &fitChisq_, fitWorkSpace_) )
		AMErrorMon::alert(this, -34, "A mathematical error occurred while fitting the correlated shift values to a quadratic curve. Please report this problem to the REIXS Acquaman developers");
	else
		for(int j=0; j<sizeY; j++)
			// The new quadratic has coefficients in fitC_
			rv[j] = lround( gsl_vector_get(fitC_, 0) + gsl_vector_get(fitC_, 1)*j + gsl_vector_get(fitC_, 2)*j*j );

	return rv;
}


#include "analysis/REIXS/REIXSXESImageABEditor.h"

QWidget * REIXSXESImageAB::createEditorWidget()
{
	return new REIXSXESImageABEditor(this);
}




