#ifndef AMDATASOURCE_H
#define AMDATASOURCE_H

#include <QObject>
#include <QMap>
#include "dataman/AMAxisInfo.h"
#include "dataman/AMnDIndex.h"

class AMDataSource;

/// This class acts as a proxy to emit signals for AMDataSource. You can receive the dataChanged() signal by hooking up to AMDataSource::signalSource().  You should never need to create an instance of this class directly.
/*! To allow classes that implement AMDataSource to also inherit QObject, AMDataSource does NOT inherit QObject.  However, it still needs a way to emit signals notifying of changes to the data, which is the role of this class.
  */
class AMDataSourceSignalSource : public QObject {
	Q_OBJECT
public:
	AMDataSource* dataSource() const { return data_; }

protected slots:
	void emitValuesChanged() { emit valuesChanged(); }
	void emitSizeChanged() { emit sizeChanged(); }
	void emitInfoChanged() { emit infoChanged(); }

protected:
	AMDataSourceSignalSource(AMDataSource* parent);
	AMDataSource* data_;
	friend class AMDataSource;

signals:
	void valuesChanged();
	void sizeChanged();
	void infoChanged();
};

class AMNumber;

/// This class defines the interface for a multi-dimensional dataset, which can be plotted and analyzed.  To implement the interface, classes must provide information about their axes (rank and size, name, and optionally description and units), information about the state/validity of the data, and notification when any of this changes.
class AMDataSource
{
public:
	/// Describes the state of the data
	enum State { InvalidState, ReadyState, ProcessingState };

	/// Implementing classes must be sure to call this constructor in their constructor initialization.
	AMDataSource();
	/// Destructor
	virtual ~AMDataSource();

	/// Observers can connect to the valuesChanged(), sizeChanged(), or infoChanged() signals of this object to receive notification that the data has changed in some way (ie: in value/validity, in rank or size, or in axis descriptions, respectively)
	AMDataSourceSignalSource* signalSource() const { return signalSource_; }



	// State of the data
	//////////////////////////
	/// Indicates the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return ReadyState when they have valid data, or ProcessingState if they're still working on it.
	virtual State state() const { return InvalidState; }
	/// Convenience function that returns true when the data's state() is in the ReadyState. This means that value() will return valid numbers (assuming the axis parameters are in range, etc.)  It's not necessary to re-implement this function.
	virtual bool isValid() const { return state() == ReadyState; }


	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const = 0;

	// Following can all be determined from above. Include anyway for convenience?
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const = 0; // { return axes_.count(); }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const = 0; // { AMnDIndex s(); foreach(AMAxis a, axes_) s << a.count(); return s; }
	/// Returns a bunch of information about a particular axis.
	virtual AMAxisInfo axisAt(int axisNumber) const = 0;
	/// Returns a bunch of information about a particular axis.
	virtual AMAxisInfo axisNamed(const QString& axisName) const = 0;


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const = 0;
	/// Returns the dependent value for a set of axis indexes, where the indices are specified by axis name. This generally will not be as fast as the other version of value().
	virtual AMNumber value(const QMap<QString, int>& axisNamesAndIndexes) const = 0;

	// Include here, or as part of AMAxisInfo ==> AMAxis ?
	/////////////////////
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) = 0;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis name and index)
	virtual AMNumber axisValue(const QString& axisName, int index) = 0;


protected:
	/// Subclasses must call this when the data has changed (in value, or validity). It allows connected plots and analysis blocks to update / re-analyze.
	void emitValuesChanged() { signalSource_->emitValuesChanged(); }
	/// Subclasses must call this when the data has changed (in rank or size). SHOULD THIS EVER HAPPEN? Disallow?
	void emitSizeChanged() { signalSource_->emitSizeChanged(); }
	/// Subclasses must call this when the descriptions of the axes have changed (units, description, etc.)
	void emitInfoChanged() { signalSource_->emitInfoChanged(); }

private:
	/// QObject proxy for emitting signals. (This interface class can't emit directly, because it doesn't want to inherit QObject.)
	AMDataSourceSignalSource* signalSource_;
	// friend class AMDataSourceSignalSource;
};

#endif // AMDATASOURCE_H
