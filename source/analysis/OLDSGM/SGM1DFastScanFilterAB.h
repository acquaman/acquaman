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


#ifndef SGM1DFASTSCANFILTERAB_H
#define SGM1DFASTSCANFILTERAB_H

#include "analysis/AMStandardAnalysisBlock.h"

class SGM1DFastScanFilterAB : public AMStandardAnalysisBlock
{
public:
Q_OBJECT

Q_CLASSINFO("AMDbObject_Attributes", "description=1D Running Average Filter Block")

public:
	virtual ~SGM1DFastScanFilterAB();
	Q_INVOKABLE SGM1DFastScanFilterAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
		- the rank() of that input source must be 1 (one-dimensional)
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

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

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// helper function to clear the cachedValues_
	void invalidateCache();

protected:

	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.
	AMAxisInfo inputAxis_;
	mutable QVector<AMNumber> cachedValues_;
	mutable QVector<AMNumber> cachedAxisValues_;
	mutable bool cacheCompletelyInvalid_;


	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
};

#endif // SGM1DFASTSCANFILTERAB_H
