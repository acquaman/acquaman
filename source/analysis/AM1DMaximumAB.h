#ifndef AM1DMAXIMUMAB_H
#define AM1DMAXIMUMAB_H

#include "analysis/AMStandardAnalysisBlock.h"

class AM1DMaximumAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Maximum Block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DMaximumAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM1DMaximumAB();

	/// Returns a description.
	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid, we expect one 1D data source. The empty list (no inputs) must always be valid.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;
	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }

	/// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
	bool canAnalyze() const { return canAnalyze_; }

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
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	/// Returns the cached range of the data contained within the data source.  This is always valid because it is always recomputed when the data is recomputed.
	virtual AMRange dataRange() const { return cachedDataRange_; }

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

	/// Computes the cached data for access getters value() and values().
	void computeCachedValues() const;

protected:
	/// The data source to be analyzed.
	AMDataSource *data_;

	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;
	/// The vector holding the data.
	mutable QVector<double> cachedData_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;
};

#endif // AM1DMAXIMUMAB_H
