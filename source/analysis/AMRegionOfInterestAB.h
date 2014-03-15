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

	/// Set the data source inputs.  Requires one data source.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;

	/// Re-implemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name()
	bool loadFromDb(AMDatabase *db, int id);

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

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	/// Holds the 1D spectrum data source.
	AMDataSource *spectrum_;
	/// Holds the range that should be binned down.
	AMRange binningRange_;
};

#endif // AMREGIONOFINTERESTAB_H
