#include "REIXSXESImageAB.h"

#include "util/AMErrorMonitor.h"

REIXSXESImageAB::REIXSXESImageAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{

	sumRangeMin_ = 2;
	sumRangeMax_ = 61;
	correlationCenterPx_ = 512;
	correlationHalfWidth_ = 40;
	liveCorrelation_ = false;
	// shift values can start out empty.

	inputSource_ = 0;
	cacheCompletelyInvalid_ = false;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	connect(&callCorrelation_, SIGNAL(executed()), this, SLOT(correlateNow()));
	setDescription("XES Analyzed Spectrum");

	// Fitting structures (unallocated)
	fitWorkSpace_ = 0;
	fitXX_ = 0;
	fitY_ = 0;
	fitWeight_ = 0;
	fitC_ = 0;
	fitCov_ = 0;
}

REIXSXESImageAB::REIXSXESImageAB(AMDatabase *db, int id) :
	AMStandardAnalysisBlock("tempName", 0)
{
	inputSource_ = 0;
	cacheCompletelyInvalid_ = false;

	// leave sources_ empty for now.

	axes_ << AMAxisInfo("invalid", 0, "No input data");

	loadFromDb(db, id);
	// will restore the parameters sumRangeMin_, sumRangeMax_, correlation settings, and shift values. We'll remain invalid until we get connected to a data source.
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
	invalidateCache();
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageAB::setSumRangeMax(int sumRangeMax)
{
	if(sumRangeMax == sumRangeMax_)
		return;

	sumRangeMax_ = sumRangeMax;
	invalidateCache();
	reviewState();

	emitValuesChanged();
	setModified(true);
}

void REIXSXESImageAB::setShiftValues(const AMIntList &shiftValues)
{
	if(shiftValues == shiftValues_)
		return;

	shiftValues_ = shiftValues;
	invalidateCache();	// could change all our cached values
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

void REIXSXESImageAB::invalidateCache()
{
	if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size) {
		cachedValues_ = QVector<AMNumber>(axes_.at(0).size);	// everything in there is now AMNumber::Null.
		cacheCompletelyInvalid_ = true;
	}
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
		releaseFittingStructures();
	}

	// we know that this will only be called with one valid input source
	else {
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;

		axes_[0] = inputSource_->axisInfoAt(0);	// take the X axis info from the data source.

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		releaseFittingStructures();	// harmless if don't exist yet.
		allocateFittingStructures(inputSource_->size(1));
	}


	invalidateCache();
	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
}


AMNumber REIXSXESImageAB::value(const AMnDIndex &indexes, bool doBoundsChecking) const
{
	if((indexes.rank() != 1))
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	// x pixel value:
	int i = indexes.i();
	// Max x pixel value:
	int maxI = inputSource_->size(0);

	if(doBoundsChecking)
		if(((unsigned)i >= (unsigned)axes_.at(0).size))
			return AMNumber(AMNumber::OutOfBoundsError);

	AMNumber rv = cachedValues_.at(i);
	// if we haven't calculated this sum yet, the cached value will be invalid. Sum using shifting, and store.
	if(!rv.isValid()) {

		double newVal = 0.0;
		int contributingRows = 0;
		for(int j=sumRangeMin_; j<=sumRangeMax_; j++) { // loop through rows
			int sourceI = i - shiftValues_.at(j);
			if(sourceI < maxI && sourceI >= 0) {
				newVal += double(inputSource_->value(AMnDIndex(sourceI, j), false));
				contributingRows++;
			}
		}

		// normalize by dividing by the number of rows that contributed. Since we want to keep the output in units similar to raw counts, multiply by the nominal (usual) number of contributing rows.
		// Essentially, this normalization prevents columns near the edge that miss out on some rows due to shifting from being artificially suppressed.  For inner columns, contributingRows will (sumRangeMax_ - sumRangeMin_ + 1).
		newVal = newVal * double(sumRangeMax_ - sumRangeMin_ + 1) / double(contributingRows);

		cachedValues_[i] = newVal;
		cacheCompletelyInvalid_ = false;
		return newVal;
	}
	// otherwise return the value we have cached.
	else
		return rv;
}

AMNumber REIXSXESImageAB::axisValue(int axisNumber, int index, bool doBoundsChecking) const
{
	if((axisNumber != 0))
		return AMNumber(AMNumber::DimensionError);

	if(doBoundsChecking) {
		if(((unsigned)index >= (unsigned)axes_.at(0).size))
			return AMNumber(AMNumber::OutOfBoundsError);
	}

	return index;
}

void REIXSXESImageAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	// invalidate the cache. We're not summing straight down, so values outside of the start - end range may change; we just invalidate everything.
	invalidateCache();
	emitValuesChanged();
}

void REIXSXESImageAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != inputSource_->size(0)) {
		axes_[0].size = inputSource_->size(0);
		cachedValues_.resize(axes_.at(0).size);	// resize() will fill in with default-constructed value for AMNumber(), which is AMNumber::Null.
		emitSizeChanged(0);
	}

	// the fitting structures have to match the vertical size of the image.
	releaseFittingStructures();
	allocateFittingStructures(inputSource_->size(1));

	// because of the shifting before summing, this could change values outside of the added/removed region; just invalidate everything.
	invalidateCache();
	// If the sumRangeMin/sumRangeMax were out of range before, they might be valid now.
	reviewState();
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
	int midY = sizeY/2;

	int minShift = -correlationHalfWidth_;
	int maxShift = correlationHalfWidth_;

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
					correlationSum += double(inputSource_->value(AMnDIndex(i,midY), false)) * double(inputSource_->value(AMnDIndex(i+shift, j), false));
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
