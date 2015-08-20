#ifndef AM0DTIMESTAMPAB_H
#define AM0DTIMESTAMPAB_H

#include "analysis/AMStandardAnalysisBlock.h"
#include "beamline/AMTime.h"

class AM0DTimestampAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=0D Timestamp Block")

public:
	/// Constructor.
	Q_INVOKABLE AM0DTimestampAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM0DTimestampAB();

	/// Returns the times stored in this analysis block, for the input data source.
	QList<QDateTime> dataStored() const { return dataStored_; }
	/// Returns the total number of elements stored.
	int dataStoredCount() const { return dataStored_.count(); }
	/// Returns the maximum number of elements stored.
	int dataStoredCountMax() const { return dataMax_; }
	/// Returns the current time value.
	int timeValue() const { return timeValue_; }
	/// Returns the current time units.
	AMTime::Units timeUnits() const { return timeUnits_; }
	/// Returns bool indicating whether time filtering is enabled.
	bool timeFilteringEnabled() const { return timeFilteringEnabled_; }

	/// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 1."
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;
	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 0; }
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are out of range, are incomplete, or the data is not ready."
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Returns the block of values from indexStart to indexEnd, copied into outputValues. Returns false if the provided AMnDIndex values have the wrong dimension or if they are out of range.
	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;
	/// Returns the independent value at the given axis and index.
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const;

signals:
	/// Notifier that the max number of data points stored has changed.
	void dataStoredMaxChanged(int newMax);
	/// Notifier that the time filtering status has changed.
	void timeFilteringEnabled(bool isEnabled);
	/// Notifier that the time value has changed.
	void timeValueChanged(int newValue);
	/// Notifier that the time units have changed.
	void timeUnitsChanged(const AMTime::Units &newUnits);

public slots:
	/// Sets the maximum number of elements stored.
	void setDataStoredCountMax(int newMax);
	/// Enables time filtering.
	void enableTimeFiltering(bool isEnabled);
	/// Sets the time value.
	void setTimeValue(int newValue);
	/// Sets the time units.
	void setTimeUnits(const AMTime::Units &newUnits);

protected slots:
	/// Handles adding the new value to the dataStored_ for this particular data source.
	void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Handles checking whether this AB is valid if the state of any sources changes.
	void onInputSourcesStateChanged();

protected:
	/// Converts the millisecond argument to another TimeUnit.
	double convertTimeValue(double mseconds, const AMTime::Units &newUnits) const;
	/// Helper function that reviews what this AB should emit as changed indices.
	void reviewValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Helper function that reviews this AB's current state.
	void reviewState();

protected:
	/// List storing the times saved in this analysis block.
	QList<QDateTime> dataStored_;
	/// The maximum number of items stored in this analysis block.
	int dataMax_;
	/// The datetime of the latest data source value update.
	QDateTime latestUpdate_;
	/// The current time value.
	int timeValue_;
	/// The current time units.
	AMTime::Units timeUnits_;
	/// Bool indicating whether time filtering is enabled.
	bool timeFilteringEnabled_;
};

#endif // AM0DTIMESTAMPAB_H
