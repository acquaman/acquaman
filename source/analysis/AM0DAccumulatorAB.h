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


#ifndef AM0DACCUMULATORAB_H
#define AM0DACCUMULATORAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

class AM0DAccumulatorAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=0D Accumulator Block")

public:
	/// Constructor.
	Q_INVOKABLE AM0DAccumulatorAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM0DAccumulatorAB();

	/// Returns the data stored in this analysis block, for the input data source.
	QList<double> dataStored() const;
	/// Returns the number of data elements stored in this analysis block.
	int dataStoredCount() const;
	/// Returns the maximum number of data elements stored in this analysis block.
	int dataStoredCountMax() const;

	/// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 1 and the source must have rank 0.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;
	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 0; }
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are out of range, are incomplete, or the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Returns the block of values from indexStart to indexEnd, copied into outputValues. Returns false if the provided AMnDIndex values have the wrong dimension or if they are out of range.
	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;
	/// Returns the independent value at the given axis and index.
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

signals:
	/// Emitted when the maximum number of elements stored has been changed.
	void dataStoredMaxChanged(int newMax);

public slots:
	/// Sets the maximum number of elements stored.
	void setDataStoredCountMax(int newMax);

protected slots:
	/// Handles adding the new value to the dataStored_ for this particular data source.
	void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Handles checking whether this AB is valid if the state of any sources changes.
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);
	/// Helper function that reviews this AB's current state.
	void reviewState();

protected:
	/// Data stored in this analysis block.
	QList<double> dataStored_;
	/// The maximum number of items stored in this analysis block.
	int dataMax_;

};

#endif // AM0DACCUMULATORAB_H
