/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMDEADTIMEAB_H
#define AMDEADTIMEAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block accepts one 1D input data source and does a dead time correction to all values using a 0D input data source.  The dead time data source must be expressed as a percentage.  The output data source is a 1D data source.
class AMDeadTimeAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=Dead Time Correction Block")

public:
	/// Constructor.
	Q_INVOKABLE AMDeadTimeAB(const QString &outputName = "InvalidInput", QObject *parent = 0);

	/// Description.
	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
	- the rank() of that input source must be 2 (two-dimensiona)
	*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.  Order needs to be spectra then ICR then OCR.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes, bool doBoundsChecking = true) const;

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

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	// Holds the 1D spectrum data source.
	AMDataSource *spectra_;
	// Holds the 0D input count rate data source.
	AMDataSource *icr_;
	// Holds the 0D output count rate data source.
	AMDataSource *ocr_;
};

#endif // AMDEADTIMEAB_H
