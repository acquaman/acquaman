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


#ifndef ST0DTIMESTAMPAB_H
#define ST0DTIMESTAMPAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

#include "StripTool2/STTime.h"

class ST0DTimestampAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=0D Timestamp Block")

public:
	/// Constructor.
	Q_INVOKABLE ST0DTimestampAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~ST0DTimestampAB();

	/// Returns the times stored in this analysis block, for the input data source.
	QList<QDateTime> dataStored() const;
	/// Returns the total number of elements stored.
	int dataStoredCount() const;
	/// Returns the maximum number of elements stored.
	int dataStoredCountMax() const;
	/// Returns the current time value.
	int timeValue() const;
	/// Returns the current time units.
	STTime::Units timeUnits() const;
	/// Returns bool indicating whether time filtering is enabled.
	bool timeFilteringEnabled() const;

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
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

signals:
	void dataStoredMaxChanged(int newMax);
	void timeFilteringEnabled(bool isEnabled);
	void timeValueChanged(int newValue);
	void timeUnitsChanged(STTime::Units newUnits);

public slots:
	/// Sets the maximum number of elements stored.
	void setDataStoredCountMax(int newMax);
	/// Enables time filtering.
	void enableTimeFiltering(bool isEnabled);
	/// Sets the time value.
	void setTimeValue(int newValue);
	/// Sets the time units.
	void setTimeUnits(STTime::Units newUnits);

protected slots:
	/// Handles adding the new value to the dataStored_ for this particular data source.
	void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Handles checking whether this AB is valid if the state of any sources changes.
	void onInputSourcesStateChanged();

protected:
	/// Converts the millisecond argument to another TimeUnit.
	double convertTimeValue(double mseconds, STTime::Units newUnits) const;
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
	STTime::Units timeUnits_;
	/// Bool indicating whether time filtering is enabled.
	bool timeFilteringEnabled_;

};

#endif // ST0DTIMESTAMPAB_H
