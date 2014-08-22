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


#ifndef AMXYSCATTERPVDATASOURCE_H
#define AMXYSCATTERPVDATASOURCE_H

#include <QObject>

#include "AMDataSource.h"
#include "beamline/AMProcessVariable.h"

class AMXYScatterPVDataSource : public QObject, public AMDataSource
{
	Q_OBJECT
public:
 	virtual ~AMXYScatterPVDataSource();
	explicit AMXYScatterPVDataSource(const AMProcessVariable *x, const AMProcessVariable *y, const QString& name, QObject *parent = 0);

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "XY Scatter Process Variable Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return x_->hasValues() && y_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		return axes_;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 1; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return 1; }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be 0.
	virtual int size(int axisNumber) const { Q_UNUSED(axisNumber) return 1; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be be 0.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { Q_UNUSED(axisNumber) return axes_.at(0); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.)
	virtual int idOfAxis(const QString& axisName)
	{
		if(axisName == axes_.at(0).name)
			return 0;

		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const
	{
		if(!x_->isConnected() || !y_->isConnected())
			return AMNumber();
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);
		if((unsigned)indexes.i() != 0)
			return AMNumber(AMNumber::OutOfBoundsError);

		return y_->lastValue(indexes.i());
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		Q_UNUSED(axisNumber)

		if(!x_->isConnected() || !y_->isConnected())
			return AMNumber();
		if(axisNumber != 0)
			return AMNumber(AMNumber::DimensionError);
		if ((unsigned)index != 0)
			return AMNumber(AMNumber::OutOfBoundsError);

		return x_->lastValue(index);
	}

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged() { emitValuesChanged(); }
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged() { emitStateChanged(state()); }

protected:
	/// The control being used as a data source.
	const AMProcessVariable *x_;
	const AMProcessVariable *y_;

	/// A QList of AMAxisInfo: caches a single AMAxisInfo for our one axis, so that axes() can be fast.
	QList<AMAxisInfo> axes_;

};

#endif // AMXYSCATTERPVDATASOURCE_H
