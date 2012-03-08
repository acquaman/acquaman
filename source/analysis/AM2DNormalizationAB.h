#ifndef AM2DNORMALIZATIONAB_H
#define AM2DNORMALIZATIONAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!	This analysis block accepts two 2D input data sources and normalizes one using the other.
	The output data is the same size as the input sources.  There is an assumed order for the
	data sources and that is:

		1) Data
		2) Normalizer
  */
class AM2DNormalizationAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=2D Normalization Block")

public:
	/// Constructor.
	Q_INVOKABLE AM2DNormalizationAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - two 2D data sources
		- For proper results, order must be the data source with real data and the second must be the normalizer.
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	// Data Retrieval

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
	/// Pointer to the data source that will be analyzed.
	AMDataSource *data_;
	/// Pointer to the data source that is the normalizer.
	AMDataSource *normalizer_;

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
};

#endif // AM2DNORMALIZATIONAB_H
