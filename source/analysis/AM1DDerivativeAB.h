#ifndef AM1DDERIVATIVEAB_H
#define AM1DDERIVATIVEAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts a single 1D input data source and calculates the derivative.  The output data is the same size as the input data source.  It uses the backward difference method (except for the first data point in the series, which uses the forward difference method.)
class AM1DDerivativeAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Derivative Block")

public:
	Q_INVOKABLE AM1DDerivativeAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
		- the rank() of that input source must be 2 (two-dimensiona)
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes, bool doBoundsChecking = true) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const;

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
	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
};

#endif // AM1DDERIVATIVEAB_H
