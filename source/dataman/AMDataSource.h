#ifndef AMDATASOURCE_H
#define AMDATASOURCE_H

#include <QObject>
#include <QMap>
#include <QSet>
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
	void emitValuesChanged(const AMnDIndex& start, const AMnDIndex& end) { emit valuesChanged(start, end); }
	void emitSizeChanged(int axisNumber) { emit sizeChanged(axisNumber); }
	void emitAxisInfoChanged(int axisNumber) { emit axisInfoChanged(axisNumber); }
	void emitInfoChanged() { emit infoChanged(); }
	void emitStateChanged(int newDataState) { emit stateChanged(newDataState); }
	void emitDeleted() { emit deleted(data_); }

protected:
	AMDataSourceSignalSource(AMDataSource* parent);
	AMDataSource* data_;
	friend class AMDataSource;

signals:
	/// Indicates that the data value()s within the n-dimensional region from \c start to \c end may have changed. If \c start is invalid (!isValid()), it means that all data values might have changed.
	void valuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Emitted when the size of the axis \c axisNumber has changed.  If \c axisNumber = -1, all axis might have changed size.
	void sizeChanged(int axisNumber);
	/// Emitted when the axis information (description, units, etc.) of an axis have changed. \c axisNumber is the axis that changed, or -1 if all axes have changed.
	void axisInfoChanged(int axisNumber);
	/// Indicates that the meta-information for this data source (currently just description()) has changed.
	void infoChanged();
	/// Emitted when the AMDataSource::State state() of the data has changed.
	void stateChanged(int newState);
	/// Emitted before the data source is deleted. \c deletedSource is a pointer to the deleted source. Observers can use this to detect when AMDataSource objects no longer exist.
	/*! (In a direct signal-slot connection, the \c deletedSource will still exist, inside ~AMDataSource(), when this is called. In a queued signal-slot connection, you should assume that \c deletedSource is already deleted. */
	void deleted(void* deletedSource);
};

class AMNumber;

/// This class defines the interface for a multi-dimensional dataset, which can be plotted and analyzed.  To implement the interface, classes must provide information about their axes (rank and size, name, and optionally description and units), information about the state/validity of the data, and notification when any of this changes.
class AMDataSource
{
public:
	/// Describes the state of the data
	enum State {
		InvalidState, ///< The data is downright invalid... all calls to value() will return invalid AMNumbers.
		ReadyState, ///< The data is valid, and generally static. (However, there is no guarantee that it won't change and emit valuesChanged() )
		ProcessingState ///< Some or all of the data might be valid, but processing is ongoing and its changing. If you have to start an expensive processing operation, you might want to wait until
	};

	/// Implementing classes must be sure to call this constructor in their initialization.
	AMDataSource(const QString& name);

	/// Destructor
	virtual ~AMDataSource();

	/// Observers can connect to the valuesChanged(), sizeChanged(), or infoChanged() signals of this object to receive notification that the data has changed in some way (ie: in value/validity, in size, or in axis descriptions, respectively)
	AMDataSourceSignalSource* signalSource() const { return signalSource_; }


	/// the identifying name() of a data source can sometimes be used to select one from a set of data sources. Therefore, it's not really recommended to change the name after a data source is created.
	QString name() const { return name_; }
	/// The description() of a data source is a human-readable, free-form string.
	QString description() const { return description_; }
	/// Descriptions can be changed at will, and the data source will emit infoChanged() when this happens.
	void setDescription(const QString& description) { description_ = description; emitInfoChanged(); }


	// State of the data
	//////////////////////////
	/// Indicates the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return ReadyState when they have valid data, or ProcessingState if they're still working on it.
	virtual State state() const { return InvalidState; }
	/// Convenience function that returns true when the data's state() is in the ReadyState. This means that value() will return valid numbers (assuming the axis parameters are in range, etc.)  It's not necessary to re-implement this function.
	virtual bool isValid() const { return state() != InvalidState; }


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
	/// Returns the number of an axis, by name. (By number, we mean the index of the axis. We called it number to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.
	virtual int numberOfAxis(const QString& axisName) = 0;


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

	// Observers
	//////////////////////////

	/// Call this to inform a data source that you are observing it, and currently depending on it.
	/*! In the classic Observer-Observable design pattern, data sources are observables.  However, to allow ANY object to observe an AMDataSource, we implement this pattern using signals (signalSource()'s valuesChanged(), sizeChanged(), axisInfoChanged(), etc.).  Still, in some cases its important to know if other objects are depending on this data source for values.  This function allows an object to register a long-term interest in this data source.  You can call observerCount() to find out how many observers a data source has.  Call deregisterObserver() to release this interest.

	  Calling registerObserver() twice with the same object does nothing on the second time.

	  \note By convention, plots and views do not normally register as observers even though they display an AMDataSource's values. However, other data sources and analysis blocks that absolutely require access to this object's data MUST register.
	  */
	void registerObserver(void* observer) {
		observers_ << observer;
	}

	/// The number of observers that depend on this data source
	int observerCount() const {
		return observers_.count();
	}

	/// Call this to inform a data source that you're no longer dependent on its data.
	void deregisterObserver(void* observer) {
		observers_.remove(observer);
	}


protected:

	/// Subclasses must call this when the data has changed (in value, or validity). It allows connected plots and analysis blocks to update / re-analyze. \c start and \c end specify the range of values that have changed. Use an invalid index (AMnDIndex()) for \c start to specify all data might have changed.
	void emitValuesChanged(const AMnDIndex& start = AMnDIndex(), const AMnDIndex& end = AMnDIndex()) { signalSource_->emitValuesChanged(start, end); }
	/// Subclasses must call this when the data has changed (in size). \c axisNumber specifies which axis changed, or -1 to indicate they all might have.  Changing rank (dimensionality) is not allowed.
	void emitSizeChanged(int axisNumber = -1) { signalSource_->emitSizeChanged(axisNumber); }
	/// Subclasses must call this when the descriptions of the axes have changed (units, description, etc.). \c axisNumber specifies which axis changed, or -1 to indicate they all might have.
	void emitAxisInfoChanged(int axisNumber = -1) { signalSource_->emitAxisInfoChanged(axisNumber); }
	/// This is emitted when the meta-info changes. (Right now, this only includes a data source's description() )
	void emitInfoChanged() { signalSource_->emitInfoChanged(); }
	/// Subclasses must call this when the state() of the data changes
	void emitStateChanged(State newDataState) { signalSource_->emitStateChanged(newDataState); }

	/// identifying name for this data source
	QString name_;
	/// Human-readable description for it
	QString description_;
	/// Set of observers
	QSet<void*> observers_;


private:
	/// QObject proxy for emitting signals. (This interface class can't emit directly, because it doesn't want to inherit QObject.)
	AMDataSourceSignalSource* signalSource_;
};

#endif // AMDATASOURCE_H
