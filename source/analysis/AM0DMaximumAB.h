#ifndef AM0DMAXIMUMAB_H
#define AM0DMAXIMUMAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!
  This analysis block accepts one 1D data source and provides a single value for the data
  source's maximum value, and the corresponding axis value.
  */
class AM0DMaximumAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=0D Maximum Block")

public:
	/// Constructor.
	Q_INVOKABLE AM0DMaximumAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM0DMaximumAB();

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
	/// Copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues. The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range). The provided pointer must contain enough space for all the requested values.
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Copies a block of values from \c startIndex to \c endIndex in \c outputValues. The provided pointer must contain enough space for all the requested values.
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
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
	/// The input data source to be analyzed.
	AMDataSource *inputSource_;

	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;

	/// The maximum value.
	mutable double maxValue_;
	/// The axis value for the maximum value.
	mutable double maxAxisValue_;
};

#endif // AM0DMAXIMUMAB_H
