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


#ifndef AMPROCESSVARIABLEDATASOURCE_H
#define AMPROCESSVARIABLEDATASOURCE_H

#include <QObject>

#include "AMDataSource.h"
#include "beamline/AMProcessVariable.h"

/*!
  This class encapsulates AMProcessVariable and puts it into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Must be a scalar PV.
  */
class AM0DProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
 	virtual ~AM0DProcessVariableDataSource();
	AM0DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "Scalar Process Variable Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return data_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		return QList<AMAxisInfo>();
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 0; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.  Returns 0 if representing a scalar process variable.
	virtual int size(int axisNumber) const { Q_UNUSED(axisNumber) return 1; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { Q_UNUSED(axisNumber) return AMAxisInfo("scalar", 1); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.  Scalars have no axes and therefore will always return -1.
	virtual int idOfAxis(const QString& axisName)
	{
		Q_UNUSED(axisName)
		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient, or if the data is not ready.  If representing a scalar then provide a null AMnDIndex (ie: AMnDIndex()).
	virtual AMNumber value(const AMnDIndex &indexes) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if (indexes.rank() != 0)
			return AMNumber(AMNumber::DimensionError);

		return data_->lastValue();
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns 0.
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		Q_UNUSED(axisNumber)
		Q_UNUSED(index)

		return 0;
	}

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged() { emitValuesChanged(); }
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged() { emitStateChanged(state()); }

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
};

/*!
  This class encapsulates AMProcessVariable and puts it into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Must be an array PV.
  */
class AM1DProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
 	virtual ~AM1DProcessVariableDataSource();
	AM1DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "Array Process Variable Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return data_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		axes_[0].size = data_->count();
		return axes_;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 1; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return AMnDIndex(data_->count()); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be 0.
	virtual int size(int axisNumber) const { Q_UNUSED(axisNumber) return data_->count(); }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be be 0.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { Q_UNUSED(axisNumber) axes_[0].size = data_->count(); return axes_.at(0); }
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
		if(!data_->isConnected())
			return AMNumber();
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= data_->count())
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		return data_->lastValue(indexes.i());
	}
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const
	{
		if (!data_->isConnected())
			return false;

		if (indexStart.rank() != 1 || indexEnd.rank() != 1)
			return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexStart.i() >= data_->count() || indexStart.i() > indexEnd.i())
			return false;
#endif

		if (data_->dataType() == PVDataType::FloatingPoint)
			memcpy(outputValues, data_->lastFloatingPointValues().constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));

		else{

			int totalSize = indexStart.totalPointsTo(indexEnd);
			QVector<double> val = QVector<double>(totalSize);
			for (int i = 0, offset = indexStart.i(); i < totalSize; i++)
				val[i] = double(data_->lastIntegerValues().at(i+offset));

			memcpy(outputValues, val.constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));
		}
		return true;
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		Q_UNUSED(axisNumber)

		if(!data_->isConnected())
			return AMNumber();
		if(axisNumber != 0)
			return AMNumber(AMNumber::DimensionError);
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if ((unsigned)index >= data_->count())
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		return index*scale_;
	}

	/// Returns the current scale used for the independent axis.
	double scale() const { return scale_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double scale) { scale_ = scale; axes_[0].increment = scale; emitValuesChanged(); emitAxisInfoChanged(); }

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged() { emitValuesChanged(); }
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged() { emitStateChanged(state()); }

protected:
	/// The epics PV being used as a data source.
	const AMProcessVariable *data_;
	/// Holds the current scale.
	double scale_;
	/// A QList of AMAxisInfo: caches a single AMAxisInfo for our one axis, so that axes() can be fast.
	mutable QList<AMAxisInfo> axes_;
};

/*!
  This class encapsulates AMProcessVariable and puts it into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Must be an array PV that can be interpreted as an image with the appropriate selection of a row length.
  */
class AM2DProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
 	virtual ~AM2DProcessVariableDataSource();
	AM2DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, int rowLength, QObject *parent = 0);

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "Image Process Variable Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return data_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		axes_[0].size = length_;
		axes_[1].size = data_->count()/length_;
		return axes_;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 2; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return AMnDIndex(length_, int(data_->count()/length_)); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be be 0 or 1.
	virtual int size(int axisNumber) const { return (axisNumber == 0) ? length_ : int(data_->count())/length_; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and 1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { axes_[0].size = length_; axes_[1].size = data_->count()/length_; return axes_.at(axisNumber); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName)
	{
		if(axisName == axes_.at(0).name)
			return 0;
		if(axisName == axes_.at(1).name)
			return 1;

		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if(indexes.rank() != 2)
			return AMNumber(AMNumber::DimensionError);
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(((unsigned)indexes.i() >= (unsigned)length_ || (unsigned)indexes.j() >= data_->count()/length_))
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		return data_->lastValue(indexes.i() + indexes.j()*length_);	/// \todo Acquaman normally uses the opposite convention: the first index varies the slowest while iterating through a flat array.
	}

	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const
	{
		if (!data_->isConnected())
			return false;

		if (indexStart.rank() != 2 || indexEnd.rank() != 2)
			return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexStart.totalPointsTo(indexEnd) >= data_->count() || indexStart.i() > indexEnd.i() || indexStart.j() > indexEnd.j())
			return false;
#endif

		memcpy(outputValues, data_->lastFloatingPointValues().constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));
		return true;
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns an invalid AMNumber if not a valid selection.
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if (axisNumber > 1)
			return AMNumber(AMNumber::DimensionError);


		if (axisNumber == 0) {
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if ((unsigned)index >= (unsigned)length_)
				return AMNumber(AMNumber::OutOfBoundsError);
#endif
			return index*sx_;
		}
		else {
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if ((unsigned)index >= data_->count()/length_)
				return AMNumber(AMNumber::OutOfBoundsError);
#endif
			return index*sy_;
		}
	}

	/// Returns the current scale used for the independent axis.
	QPair<double, double> scale() const { return qMakePair(sx_, sy_); }
	/// Returns the x scale.
	double scaleX() const { return sx_; }
	/// Returns the y scale.
	double scaleY() const { return sy_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double sx, double sy) { sx_ = sx; sy_ = sy; axes_[0].increment = sx_; axes_[1].increment = sy_; emitAxisInfoChanged(); emitValuesChanged(); }
	/// Changes the x scale for the data source.
	void setScaleX(double sx) { sx_ = sx; axes_[0].increment = sx; emitAxisInfoChanged(0); emitValuesChanged(); }
	/// Changes the y scale for the data source.
	void setScaleY(double sy) { sy_ = sy; axes_[1].increment = sy; emitAxisInfoChanged(1); emitValuesChanged(); }

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged() { emitValuesChanged(); }
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged() { emitStateChanged(state()); }

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
	/// Holds the current x scale.
	double sx_;
	/// Holds the current y scale.
	double sy_;
	/// Holds the current row length.
	int length_;

	/// A QList of AMAxisInfo: caches a couple AMAxisInfo for our two axes, so that axes() can be fast.
	mutable QList<AMAxisInfo> axes_;
};

#endif // AMPROCESSVARIABLEDATASOURCE_H
