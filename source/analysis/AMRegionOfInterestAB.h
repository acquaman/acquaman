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


#ifndef AMREGIONOFINTERESTAB_H
#define AMREGIONOFINTERESTAB_H

#include "analysis/AMStandardAnalysisBlock.h"
#include "util/AMRange.h"

/// This analysis block takes a spectrum data source and sums down over a defined range.  It is specifically tailored to doing a region of interest.
class AMRegionOfInterestAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(double binningRangeUpperBound READ binningRangeUpperBound WRITE setBinningRangeUpperBound)
	Q_PROPERTY(double binningRangeLowerBound READ binningRangeLowerBound WRITE setBinningRangeLowerBound)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Region of Interest Block")

public:
	/// Constructor.
	Q_INVOKABLE AMRegionOfInterestAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AMRegionOfInterestAB();

	/// Returns the description of the analysis block.
	virtual QString infoDescription() const { return QString(); }

	/// Returns the binning range.
	const AMRange &binningRange() const { return binningRange_; }
	/// Returns the binning range's upper bound.
	double binningRangeUpperBound() const { return binningRange_.maximum(); }
	/// Returns the binning range's lower bound.
	double binningRangeLowerBound() const { return binningRange_.minimum(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*!
		- there must be one data source
	*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.  Can be anything >= 1 so I'm setting it to -1.
	virtual int desiredInputRank() const { return -1; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;
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

public slots:
	/// Sets the binning range.
	void setBinningRange(const AMRange &newRange);
	/// Sets the upper bound of the binning range.
	void setBinningRangeUpperBound(double upperBound);
	/// Sets the lower bound of the binning range.
	void setBinningRangeLowerBound(double lowerBound);

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.  Requires one data source.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);
	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	/// Computes the cached data for access getters value() and values().
	void computeCachedValues() const;

	/// Holds the 1D spectrum data source.
	AMDataSource *spectrum_;
	/// Holds the range that should be binned down.
	AMRange binningRange_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;
	/// A list of indices of values that need to be updated.  These are meant for when scans are running and data is coming in a pixel at a time.
	mutable QList<AMnDIndex> dirtyIndices_;
	/// The vector holding the data.
	mutable QVector<double> cachedData_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;
};

#endif // AMREGIONOFINTERESTAB_H
