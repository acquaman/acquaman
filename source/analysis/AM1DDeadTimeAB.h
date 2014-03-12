#ifndef AM1DDEADTIMEAB_H
#define AM1DDEADTIMEAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts one 1D input data source and does a dead time correction to all values using two 1D input data sources, Input Count Rate (ICR) and Output Count Rate (OCR).  The output data source is a 1D data source.
class AM1DDeadTimeAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Dead Time Correction Block")

public:
	/// Constructor.
 	virtual ~AM1DDeadTimeAB();
	Q_INVOKABLE AM1DDeadTimeAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	/// Description.
	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
	- the rank() of that input source must be 1 (two-dimensiona)
	*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.  Order needs to be spectra then ICR then OCR.
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

	/// Holds the 2D spectrum data source.
	AMDataSource *spectra_;
	/// Holds the 1D input count rate data source.
	AMDataSource *icr_;
	/// Holds the 1D output count rate data source.
	AMDataSource *ocr_;
};

#endif // AM1DDEADTIMEAB_H
