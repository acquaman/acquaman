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


#ifndef AM4DBINNINGAB_H
#define AM4DBINNINGAB_H

#include "analysis/AMStandardAnalysisBlock.h"

#define AM3DMAGICNUMBER -1.493920384020

/// This analysis block accepts a single 4D data source as input and outputs a 3D data source by summing across a specified axis (within a region of interest).
class AM4DBinningAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(int sumAxis READ sumAxis WRITE setSumAxis)
	Q_PROPERTY(int sumRangeMin READ sumRangeMin WRITE setSumRangeMin)
	Q_PROPERTY(int sumRangeMax READ sumRangeMax WRITE setSumRangeMax)
	Q_PROPERTY(QString analyzedName READ analyzedName WRITE setAnalyzedName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=4D Binning Block")

public:
	/// Constructor.  \c outputName is the name() for the output data source.
	virtual ~AM4DBinningAB();
	AM4DBinningAB(const QString &outputName, QObject *parent = 0);
	/// This constructor is used to reload analysis blocks directly out of the database.
	Q_INVOKABLE AM4DBinningAB(AMDatabase *db, int id);

	QString infoDescription() const { return QString("(Axis %1 from %2 to %3)").arg(sumAxis_).arg(sumRangeMin_).arg(sumRangeMax_); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source or a list of 2D data sources.
  - the rank() of that input source must be 2 (two-dimensional)
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 4; }

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

public:
	/// Set the analyzed data source name.
	void setAnalyzedName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString analyzedName() const { return analyzedName_; }
	/// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
	bool canAnalyze() const { return canAnalyze_; }
	/// Returns whether the data source can be evaluated by passing in a name.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
	bool canAnalyze(const QString &name) const;

	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing the expressions.
	virtual QWidget* createEditorWidget();

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const;


	// Analysis parameters
	///////////////////////////
	int sumAxis() const { return sumAxis_; }
	int sumRangeMin() const { return sumRangeMin_; }
	int sumRangeMax() const { return sumRangeMax_; }

	// Setting parameters
	///////////////////////////
	/// Specify the axis which is summed along. (The size() of the output thus becomes the size of the other axis.) This must be 0 or 1.
	void setSumAxis(int sumAxis) {
		if((unsigned)sumAxis >= 3)
			return;

		if(sumAxis == sumAxis_)
			return;	// no change

		sumAxis_ = sumAxis;

		// if we have a data source, set our output axisInfo to match the input source's other axis. This also changes our size.
		if(inputSource_) {

			switch (sumAxis_){

			case 0:
				axes_[0] = inputSource_->axisInfoAt(1);
				axes_[1] = inputSource_->axisInfoAt(2);
				axes_[2] = inputSource_->axisInfoAt(3);
				break;

			case 1:
				axes_[0] = inputSource_->axisInfoAt(0);
				axes_[1] = inputSource_->axisInfoAt(2);
				axes_[2] = inputSource_->axisInfoAt(3);
				break;

			case 2:
				axes_[0] = inputSource_->axisInfoAt(0);
				axes_[1] = inputSource_->axisInfoAt(1);
				axes_[2] = inputSource_->axisInfoAt(3);
				break;

			case 3:
				axes_[0] = inputSource_->axisInfoAt(0);
				axes_[1] = inputSource_->axisInfoAt(1);
				axes_[2] = inputSource_->axisInfoAt(2);
				break;
			}

			setDescription(QString("%1 summed (over %2)")
						   .arg(inputSource_->name())
						   .arg(inputSource_->axisInfoAt(sumAxis_).name));
		}

		invalidateCache();
		reviewState();

		emitSizeChanged();
		emitValuesChanged();
		emitAxisInfoChanged();
		emitInfoChanged();
		setModified(true);
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
		setModified(true);
	}

	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMax(int sumRangeMax) {
		if(sumRangeMax == sumRangeMax_)
			return;

		sumRangeMax_ = sumRangeMax;
		invalidateCache();
		reviewState();

		emitValuesChanged();
		setModified(true);
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
	/// Helper method that sets the inputSource_ pointer to the correct one based on the current state of analyzedName_.
	void setInputSource();
	/// helper function to clear the cachedValues_
	void invalidateCache() {
		if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size*axes_.at(1).size*axes_.at(2).size) {
			cachedValues_ = QVector<double>(axes_.at(0).size*axes_.at(1).size*axes_.at(2).size, AM3DMAGICNUMBER);	// everything in there is now AMNumber::Null.
			cacheCompletelyInvalid_ = true;
		}
	}

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState() {

		if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid()) {
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

	/// Cached previously-summed values.  Either they don't need to be re-calculated, or they're the magic number and do need to be recalculated.
	mutable QVector<double> cachedValues_;
	/// Optimization: invalidating the cache with invalid() requires clearing all values in it. If we've just done this, we can avoid re-doing it until there's actually something to clear.
	mutable bool cacheCompletelyInvalid_;

	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	int sumAxis_;
	int sumRangeMin_, sumRangeMax_;

	/// The name of the data source that should be analyzed.
	QString analyzedName_;
	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;
};

#endif // AM4DBINNINGAB_H
