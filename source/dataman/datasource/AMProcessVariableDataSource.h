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
	AM0DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);
	/// Destructor.
	virtual ~AM0DProcessVariableDataSource();

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const;

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const;

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const;

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const;
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const;
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.  Returns 0 if representing a scalar process variable.
	virtual int size(int axisNumber) const;
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const;
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.  Scalars have no axes and therefore will always return -1.
	virtual int idOfAxis(const QString& axisName) const;

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient, or if the data is not ready.  If representing a scalar then provide a null AMnDIndex (ie: AMnDIndex()).
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns 0.
	virtual AMNumber axisValue(int axisNumber, int index) const;

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged();

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
	/// A QList of AMAxisInfo: caches a single AMAxisInfo for our one axis, so that axes() can be fast.
	QList<AMAxisInfo> axes_;
};

/*!
  This class encapsulates AMProcessVariable and puts it into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Must be an array PV.
  */
class AM1DProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
	AM1DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);
	/// Destructor.
	virtual ~AM1DProcessVariableDataSource();

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const;

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const;

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const;

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const;
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const;
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be 0.
	virtual int size(int axisNumber) const;
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be be 0.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const;
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.)
	virtual int idOfAxis(const QString& axisName) const;

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	/// Returns the current scale used for the independent axis.
	double scale() const;

	/// Returns the current offset used for the independent axis
	double offset() const;

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double scale);

	/// Changes the offset used in the plot
	void setOffset(double offset);

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged();
	/// Handles ensuring the axis information is properly set after connecting to the PV.
	void onInitialized();

protected:
	/// The epics PV being used as a data source.
	const AMProcessVariable *data_;

	/// A QList of AMAxisInfo: caches a single AMAxisInfo for our one axis, so that axes() can be fast.
	QList<AMAxisInfo> axes_;
};

/*!
  This class encapsulates AMProcessVariable and puts it into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Must be an array PV that can be interpreted as an image with the appropriate selection of a row length.
  */
class AM2DProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
	AM2DProcessVariableDataSource(const AMProcessVariable *data, const QString& name, int rowLength, QObject *parent = 0);
	/// Destructor.
	virtual ~AM2DProcessVariableDataSource();

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const;

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const;

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const;

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const;
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const;
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be be 0 or 1.
	virtual int size(int axisNumber) const;
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and 1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const;
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName) const;

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes) const;

	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns an invalid AMNumber if not a valid selection.
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	/// Returns the current scale used for the independent axis.
	QPair<double, double> scale() const;
	/// Returns the x scale.
	double scaleX() const;
	/// Returns the y scale.
	double scaleY() const;

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double sx, double sy);
	/// Changes the x scale for the data source.
	void setScaleX(double sx);
	/// Changes the y scale for the data source.
	void setScaleY(double sy);

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged();
	/// Handles ensuring the axis information is properly set after connecting to the PV.
	void onInitialized();

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
	QList<AMAxisInfo> axes_;
};

#endif // AMPROCESSVARIABLEDATASOURCE_H
