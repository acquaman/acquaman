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


#ifndef AMORDERREDUCTIONAB_H
#define AMORDERREDUCTIONAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!
	This analysis block accepts a single data source and is capable of reducing the order
	of a particular scan dimension.  For example, if you had a 3D scan (scanRank() == 3)
	this analysis block would provide
  */
class AMOrderReductionAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString selectedName READ selectedName WRITE setSelectedName)
	Q_PROPERTY(int reducedAxis READ reducedAxis WRITE setReducedAxis)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Order Reduction Block")

public:
	/// Constructor.
	virtual ~AMOrderReductionAB();
	Q_INVOKABLE AMOrderReductionAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*!
		- only requirement is that the datasource can't be of rank 0
	  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.  Anything but rank 0 is okay so -1.
	virtual int desiredInputRank() const { return -1; }

	/// Set the selected data source name.
	void setSelectedName(const QString &name);
	/// Returns the selected data source name.  If none has been set this returns an empty string.
	QString selectedName() const { return selectedName_; }
	/// Set the reduced axis.
	void setReducedAxis(int axis);
	/// Returns the current reduced axis.  Returns -1 is one hasn't been set yet.
	int reducedAxis() const { return reducedAxis_; }

	/// Returns the current input source if there is one.  If no input source is set, this method returns 0.
	AMDataSource *currentInputSource() const { return source_; }

	// Data Retrieval

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

	//////////////////////////////////////////////

	/// Reimplement the create widget method.
	QWidget *createEditorWidget();

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);
	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
	/// Helper method that sets the source pointer to the correct data source based on the current state of selectedName().
	void setInputSources();
	/// Helper method that gets all the axes setup after the reduced axis has been modified.
	void updateAxes();
	/// Helper method that returns the proper index for the input source based on the reduced axis, input index, and a value to fill in for the reduced axis.  \note \code reducedIndex is what you want to be placed at the input index.
	AMnDIndex inputIndex(const AMnDIndex &input, int reducedIndex) const;
	/// Helper method that returns the axis of input source based on provided axis.
	int inputAxisIndex(int axisId) const;
	/// Helper method that returns an index of the rank of the analysis block by removing the reduced axis.
	AMnDIndex outputIndex(const AMnDIndex &input) const;
	/// Computes the cached data for access getters value() and values().
	void computeCachedValues() const;

	/// Pointer to the source that will be analyzed.
	AMDataSource *source_;
	/// The name of the selected data source.
	QString selectedName_;
	/// Holds the reduced axis id.
	int reducedAxis_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;
	/// The vector holding the data.
	mutable QVector<double> cachedData_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;
};

#endif // AMORDERREDUCTIONAB_H
