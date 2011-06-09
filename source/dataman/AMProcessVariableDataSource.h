#ifndef AMPROCESSVARIABLEDATASOURCE_H
#define AMPROCESSVARIABLEDATASOURCE_H

#include <QObject>

#include "AMDataSource.h"
#include "beamline/AMProcessVariable.h"

/*!
  This class encapsulates AMProcessVariable and puts them into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.  Can be a scalar PV or an array PV.
  */
class AMProcessVariableDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMProcessVariable.
	AMProcessVariableDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "Process Variable Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return data_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		QList<AMAxisInfo> axisInfo;
		if (data_->count() != 1)
			axisInfo << AMAxisInfo("array", data_->count(), "Array Index");
		return axisInfo;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return axes().count(); }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return rank() == 0 ? AMnDIndex() : AMnDIndex(axes().count()); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.  Returns 0 if representing a scalar process variable.
	virtual int size(int axisNumber) const { return rank() == 0 ? 0 : axes().at(axisNumber).size; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { return rank() == 0 ? AMAxisInfo("scalar", 1) : axes().at(axisNumber); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.  Scalars have no axes and therefore will return -1 also.
	virtual int idOfAxis(const QString& axisName)
	{
		for (int i = 0; i < axes().count(); i++)
			if (axes().at(i).name.compare(axisName) == 0)
				return i;

		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.  If representing a scalar then provide a null AMnDIndex (ie: AMnDIndex()).
	virtual AMNumber value(const AMnDIndex& indexes) const
	{
		if(!data_->isConnected())
			return AMNumber();
		// Handles scalars.
		if (indexes.rank() == 0)
			return data_->lastValue();
		// Handles arrays.
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);
		if(indexes.i() >= axes().first().size)
			return AMNumber(AMNumber::OutOfBoundsError);

		return data_->lastValue(indexes.i());
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns 0 if representing a scalar process variable.
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		Q_UNUSED(axisNumber)

		return rank() == 0 ? 0 : index*scale();
	}

	/// Returns the current scale used for the independent axis.
	double scale() const { return scale_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double scale) { scale_ = scale; onDataChanged(); }

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged();

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
	/// Holds the current scale.
	double scale_;
};

#endif // AMPROCESSVARIABLEDATASOURCE_H
