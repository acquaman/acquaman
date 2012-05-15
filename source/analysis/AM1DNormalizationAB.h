#ifndef AM1DNORMALIZATIONAB_H
#define AM1DNORMALIZATIONAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!
  This analysis block accepts two or more 1D data sources and normalizes one using the other.
  The output data is the same size as the input sources.  There is an assumed order for the
  data sources and that is:

		1) Data
		2) Choice for normalization

The actual data source used by normalization is determined by the analyzed name provided.
  */
class AM1DNormalizationAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString dataName READ dataName WRITE setDataName)
	Q_PROPERTY(QString normalizationName READ normalizationName WRITE setNormalizationName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Normalization Block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DNormalizationAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - two 1D data sources
		- For proper results, order must be the data source with real data and the second must be the normalizer.
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Set the analyzed data source name.
	void setDataName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString dataName() const { return dataName_; }
	/// Set the analyzed data source name.
	void setNormalizationName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString normalizationName() const { return normalizationName_; }
	/// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
	bool canAnalyze() const { return canAnalyze_; }
	/// Returns whether the data source can be evaluated by passing in both names, \param dataName and \param normalizationName.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
	bool canAnalyze(const QString &dataName, const QString &normalizationName) const;

	// Data Retrieval

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes, bool doBoundsChecking = true) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const;

	//////////////////////////////////////////////

	/// Reimplement the create widget method.
	QWidget *createEditorWidget();

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
	/// Helper method that sets the data_ and normalizer_ pointer to the correct data source based on the current state of analyzedName_.
	void setInputSources();

	/// Pointer to the data source that will be analyzed.
	AMDataSource *data_;
	/// Pointer to the data source that is the normalizer.
	AMDataSource *normalizer_;

	/// The name of the data source that should be analyzed.
	QString dataName_;
	/// The name of the data source that be used for normalization.
	QString normalizationName_;
	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;
};

#endif // AM1DNORMALIZATIONAB_H
