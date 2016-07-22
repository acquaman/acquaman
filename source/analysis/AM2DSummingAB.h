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


#ifndef AM2DSUMMINGAB_H
#define AM2DSUMMINGAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts a single 2D data source as input, and outputs a 1D data source by summing across a specified axis (within a region of interest).
class AM2DSummingAB : public AMStandardAnalysisBlock {
	Q_OBJECT

	Q_PROPERTY(int sumAxis READ sumAxis WRITE setSumAxis)
	Q_PROPERTY(int sumRangeMin READ sumRangeMin WRITE setSumRangeMin)
	Q_PROPERTY(int sumRangeMax READ sumRangeMax WRITE setSumRangeMax)
	Q_PROPERTY(QString analyzedName READ analyzedName WRITE setAnalyzedName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=2D Binning Block")

public:
	/// Constructor. \c outputName is the name() for the output data source.
	Q_INVOKABLE AM2DSummingAB(const QString& outputName = "InvalidInput", QObject* parent = 0);
	/// Destructor.
	virtual ~AM2DSummingAB();

	QString infoDescription() const { return QString("(Axis %1 from %2 to %3)").arg(sumAxis_).arg(sumRangeMin_).arg(sumRangeMax_); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source or a list of 2D data sources.
  - the rank() of that input source must be 2 (two-dimensional)
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 2; }

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

	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing the expressions.
	virtual QWidget* createEditorWidget();



	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	/// Returns the cached range of the data contained within the data source.  This is always valid because it is always recomputed when the data is recomputed.
	virtual AMRange dataRange() const { return cachedDataRange_; }

	// Analysis parameters
	///////////////////////////
	int sumAxis() const { return sumAxis_; }
	int sumRangeMin() const { return sumRangeMin_; }
	int sumRangeMax() const { return sumRangeMax_; }

	// Setting parameters
	///////////////////////////
	/// Specify the axis which is summed along. (The size() of the output thus becomes the size of the other axis.) This must be 0 or 1.
	void setSumAxis(int sumAxis);
	/// Set the minimum index in the region of interest.  If the sum range is beyond the size of the summed axis, the output goes invalid. The value remains as set, however.
	void setSumRangeMin(int sumRangeMin);
	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMax(int sumRangeMax);

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
	void updateInputSource(AMDataSource *dataSource = 0, const QString emptyDataSourceDescription="Sum");
	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	/// Computes the cached data for access getters value() and values().
	void computeCachedValues() const;

	/// The input source.
	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	/// The axis to sum over.
	int sumAxis_;
	/// The lower bound of the sum range.
	int sumRangeMin_;
	/// The upper bound of the sum range.
	int sumRangeMax_;

	/// The name of the data source that should be analyzed.
	QString analyzedName_;
	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;
	/// A list of indices of values that need to be updated.  These are meant for when scans are running and data is coming in a pixel at a time.
	mutable QList<AMnDIndex> dirtyIndices_;
	/// The vector holding the data.
	mutable QVector<double> cachedData_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;
};


#endif // AM2DSUMMINGAB_H
