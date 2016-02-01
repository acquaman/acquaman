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


#ifndef AM1DTIMEDDATAAB_H
#define AM1DTIMEDDATAAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

class AM1DTimedDataAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Relative Time Block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DTimedDataAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM1DTimedDataAB();

	/// Returns the data values that are to be displayed.
	QVector<double> dataValues() const;
	/// Returns the time values that are to be displayed.
	QVector<double> timeValues() const;

	/// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 2 and the sources must have rank 1.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;
	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are out of range, are incomplete, or the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Returns the block of values from indexStart to indexEnd, copied into outputValues. Returns false if the provided AMnDIndex values have the wrong dimension or if they are out of range.
	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;
	/// Returns the independent value at the given axis and index.
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

signals:

public slots:

protected slots:
	/// Increments the updateOffset_, indicating that a data update has been made.
	void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Decrements the updateOffset_, indicating that a time update has been made.
	void onTimeSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Handles checking whether this AB is valid if the state of any sources changes.
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);
	/// Helper function that reviews source update indices start and end, and handles emitting the proper indices for this AB.
	void reviewValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Helper function that reviews this AB's current state.
	void reviewState();

protected:
	/// Positive offset indicates that more data_ updates have been made, negative that there are more times_ updates. Should usually be zero.
	int updateOffset_;
	/// The data source contributing value updates.
	AMDataSource *data_;
	/// The data soruce contributing time updates.
	AMDataSource *timestamps_;
	/// The data values to be displayed.
	QVector<double> values_;
	/// The time values to be displayed.
	QVector<double> times_;

};

#endif // AM1DTIMEDDATAAB_H
