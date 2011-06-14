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

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 0; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.  Returns 0 if representing a scalar process variable.
	virtual int size(int axisNumber) const { Q_UNUSED(axisNumber) return 0; }
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

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.  If representing a scalar then provide a null AMnDIndex (ie: AMnDIndex()).
	virtual AMNumber value(const AMnDIndex &indexes, bool doBoundsChecking = true) const
	{
		Q_UNUSED(doBoundsChecking)

		if(!data_->isConnected())
			return AMNumber();
		if (indexes.rank() != 0)
			return AMNumber(AMNumber::DimensionError);

		return data_->lastValue();
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns 0.
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const
	{
		Q_UNUSED(axisNumber)
		Q_UNUSED(index)
		Q_UNUSED(doBoundsChecking)

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
		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("array", data_->count(), "Array Index");
		ai.increment = scale_;
		axisInfo << ai;
		return axisInfo;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 1; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return AMnDIndex(axes().count()); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return axes().at(axisNumber).size; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { return axes().at(axisNumber); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName)
	{
		for (int i = 0; i < axes().count(); i++)
			if (axes().at(i).name.compare(axisName) == 0)
				return i;

		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes, bool doBoundsChecking = true) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);
		if(doBoundsChecking && indexes.i() >= axes().first().size)
			return AMNumber(AMNumber::OutOfBoundsError);

		return data_->lastValue(indexes.i());
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const
	{
		Q_UNUSED(axisNumber)

		if(!data_->isConnected())
			return AMNumber();
		if(axisNumber != 0)
			return AMNumber(AMNumber::DimensionError);
		if (doBoundsChecking && index > axes().first().size)
			return AMNumber(AMNumber::OutOfBoundsError);

		return index*scale_;
	}

	/// Returns the current scale used for the independent axis.
	double scale() const { return scale_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double scale) { scale_ = scale; onDataChanged(); }

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged() { emitValuesChanged(); }
	/// Emits the state changed signal when the process variable has values or not.
	void onStateChanged() { emitStateChanged(state()); }

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
	/// Holds the current scale.
	double scale_;
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
		QList<AMAxisInfo> axisInfo;
		AMAxisInfo xaxis("x-axis", length_, "X-axis of the image");
		xaxis.increment = sx_;
		AMAxisInfo yaxis("y-axis", data_->count()/length_, "Y-axis of the image");
		yaxis.increment = sy_;
		axisInfo << xaxis << yaxis;
		return axisInfo;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 2; }
	/// Returns the size of (ie: count along) each dimension.  Returns a null AMnDIndex if it is a scalar quantity.
	virtual AMnDIndex size() const { return 1; }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return (axisNumber == 1) ? data_->count()/length_ : length_; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const {
//		axes_[0].size = length_;
	//	axes_[1].size = data_->count()/length_;
		return axes_.at(axisNumber);
	}
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName)
	{
		if(axisName == axes_.at(0).name) return 0;
		if(axisName == axes_.at(1).name) return 1;
		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex &indexes, bool doBoundsChecking = true) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if(indexes.rank() != 2)
			return AMNumber(AMNumber::DimensionError);
		if(doBoundsChecking && (indexes.i() >= axes().first().size || indexes.j() >= axes().at(1).size))
			return AMNumber(AMNumber::OutOfBoundsError);

		return data_->lastValue(indexes.i() + indexes.j()*length_);
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).  Returns an invalid AMNumber if not a valid selection.
	virtual AMNumber axisValue(int axisNumber, int index, bool doBoundsChecking = true) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if (axisNumber > 1)
			return AMNumber(AMNumber::DimensionError);
		if (doBoundsChecking && index >= axes().at(axisNumber).size)
			return AMNumber(AMNumber::OutOfBoundsError);

		if (axisNumber == 0)
			return index*sx_;
		else if (axisNumber == 1)
			return index*sy_;

		return AMNumber();
	}

	/// Returns the current scale used for the independent axis.
	QPair<double, double> scale() const { return qMakePair(sx_, sy_); }
	/// Returns the x scale.
	double scaleX() const { return sx_; }
	/// Returns the y scale.
	double scaleY() const { return sy_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double sx, double sy) { sx_ = sx; sy_ = sy; onDataChanged(); }
	/// Changes the x scale for the data source.
	void setScaleX(double sx) { sx_ = sx; onDataChanged(); }
	/// Changes the y scale for the data source.
	void setScaleY(double sy) { sy_ = sy; onDataChanged(); }

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

	QList<AMAxisInfo> axes_;
};

#endif // AMPROCESSVARIABLEDATASOURCE_H
