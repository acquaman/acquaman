/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AM2DSUMMINGAB_H
#define AM2DSUMMINGAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts a single 2D data source as input, and outputs a 1D data source by summing across a specified axis (within a region of interest).
class AM2DSummingAB : public AMStandardAnalysisBlock {
	Q_OBJECT

	Q_PROPERTY(int sumAxis READ sumAxis WRITE setSumAxis)
	Q_PROPERTY(int sumRangeMin READ sumRangeMin WRITE setSumRangeMin)
	Q_PROPERTY(int sumRangeMax READ sumRangeMax WRITE setSumRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=2D Binning Block")

public:
	/// Constructor. \c outputName is the name() for the output data source.
	AM2DSummingAB(const QString& outputName, QObject* parent = 0);
	/// This constructor is used to reload analysis blocks directly out of the database
	Q_INVOKABLE AM2DSummingAB(AMDatabase* db, int id);


	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
  - the rank() of that input source must be 2 (two-dimensiona)
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);



	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing the expressions.
	virtual QWidget* createEditorWidget();



	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual inline AMNumber value(const AMnDIndex& indexes, bool doBoundsChecking = true) const {
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);

		if(!isValid())
			return AMNumber(AMNumber::InvalidError);

		if(doBoundsChecking)
			if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
				return AMNumber(AMNumber::OutOfBoundsError);

		AMNumber rv = cachedValues_.at(indexes.i());
		// if we haven't calculated this sum yet, the cached value will be invalid. Sum and store.
		if(!rv.isValid()) {
			double newVal = 0.0;	/// \todo preserve int/double nature of values
			if(sumAxis_ == 0)
				for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
					newVal += (double)inputSource_->value(AMnDIndex(i, indexes.i()), false);
			else
				for(int i=sumRangeMin_; i<=sumRangeMax_; i++)
					newVal += (double)inputSource_->value(AMnDIndex(indexes.i(), i), false);

			cachedValues_[indexes.i()] = newVal;
			cacheCompletelyInvalid_ = false;
			return newVal;
		}
		// otherwise return the value we have.
		else
			return rv;
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual inline AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const {

		if(!isValid())
			return AMNumber(AMNumber::InvalidError);

		if(axisNumber != 0)
			return AMNumber(AMNumber::DimensionError);

		int otherAxis = (sumAxis_ == 0) ? 1 : 0;

		return inputSource_->axisValue(otherAxis, index, doBoundsChecking);
	}


	// Analysis parameters
	///////////////////////////
	int sumAxis() const { return sumAxis_; }
	int sumRangeMin() const { return sumRangeMin_; }
	int sumRangeMax() const { return sumRangeMax_; }

	// Setting parameters
	///////////////////////////
	/// Specify the axis which is summed along. (The size() of the output thus becomes the size of the other axis.) This must be 0 or 1.
	void setSumAxis(int sumAxis) {
		if((unsigned)sumAxis >= 2)
			return;

		if(sumAxis == sumAxis_)
			return;	// no change

		sumAxis_ = sumAxis;
		int otherAxis = (sumAxis_ == 0) ? 1 : 0;

		// if we have a data source, set our output axisInfo to match the input source's other axis. This also changes our size.
		if(inputSource_) {
			axes_[0] = inputSource_->axisInfoAt(otherAxis);
			setDescription(QString("%1 summed (over %2)")
						   .arg(inputSource_->name())
						   .arg(inputSource_->axisInfoAt(sumAxis_).name));
		}

		invalidateCache();
		reviewState();

		emitSizeChanged(0);
		emitValuesChanged();
		emitAxisInfoChanged(0);
		emitInfoChanged();
	}

	/// Set the minimum index in the region of interest.  If the sum range is beyond the size of the summed axis, the output goes invalid. The value remains as set, however.
	void setSumRangeMin(int sumRangeMin) {
		// no change
		if(sumRangeMin == sumRangeMin_)
			return;

		sumRangeMin_ = sumRangeMin;
		invalidateCache();
		reviewState();

		emitValuesChanged();
	}

	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMax(int sumRangeMax) {
		if(sumRangeMax == sumRangeMax_)
			return;

		sumRangeMax_ = sumRangeMax;
		invalidateCache();
		reviewState();

		emitValuesChanged();
	}




signals:

public slots:

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();


protected:
	/// Cached previously-summed values.  Either they don't need to be re-calculated, or they're AMNumber::Null and do need to be recalculated.
	mutable QVector<AMNumber> cachedValues_;
	/// Optimization: invalidating the cache with invalid() requires clearing all values in it. If we've just done this, we can avoid re-doing it until there's actually something to clear.
	mutable bool cacheCompletelyInvalid_;

	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	int sumAxis_;
	int sumRangeMin_, sumRangeMax_;

	/// helper function to clear the cachedValues_
	void invalidateCache() {
		if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size) {
			cachedValues_ = QVector<AMNumber>(axes_.at(0).size);	// everything in there is now AMNumber::Null.
			cacheCompletelyInvalid_ = true;
		}
	}


	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState() {

		if(inputSource_ == 0 || !inputSource_->isValid()) {
			setState(AMDataSource::InvalidFlag);
			return;
		}

		int s = inputSource_->size(sumAxis_);

		if(sumRangeMin_ >= s || sumRangeMax_ >= s) {
			setState(AMDataSource::InvalidFlag);
		}
		else
			setState(0);

	}


};


#endif // AM2DSUMMINGAB_H
