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


#ifndef AMDATASOURCE_H
#define AMDATASOURCE_H

#include <QObject>
#include <QMap>
#include <QSet>
#include "dataman/AMAxisInfo.h"
#include "dataman/AMnDIndex.h"
#include "util/AMRange.h"

class AMDataSource;

#define AMDATASOURCE_VALUES_BASE_IMPLEMENTATION_CALLED 595001
#define AMDATASOURCE_AXISVALUES_BASE_IMPLEMENTATION_CALLED 595002

/// This class acts as a proxy to emit signals for AMDataSource. You can receive the dataChanged(), sizeChanged(), etc. signals by hooking up to AMDataSource::signalSource().  You should never need to create an instance of this class directly.
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
	virtual ~AMDataSourceSignalSource();
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
	/// Emitted when the state() flags of the data have changed.
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
	/// Flags to describe the state of the data
	enum StateFlags {
		InvalidFlag = 1, ///< The dataset is downright invalid... all calls to value() will return invalid AMNumbers.
		ProcessingFlag = 2 ///< Some or all of the data might be valid, but processing is ongoing and the data is changing. If you have to start an expensive processing operation, you might want to wait until this flag goes away.
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

	/// The infoDescription contains the values of any configurable settings that the data source (especially an analysis block) is using
	virtual QString infoDescription() const { return QString(); }

	/// Returns the units.
	QString units() const { return units_; }


	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "Generic Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return InvalidFlag; }
	/// Convenience function that returns true when the data's state() does not include the InvalidFlag. This means that value() will return valid numbers (assuming the axis parameters are in range, etc.)  It's not necessary to re-implement this function.
	virtual bool isValid() const { return !(state() & InvalidFlag); }


	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const = 0;

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast. Using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const = 0;
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const = 0;
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const = 0;
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const = 0;
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName) const = 0;


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const = 0;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! The base-class implementation simply calls value() repeatedly, so you should absolutely re-implement this for better performance.

	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

\code
int outputSize = indexStart.totalPointsTo(indexEnd);
\endcode
*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index).
	virtual AMNumber axisValue(int axisNumber, int index) const = 0;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
	virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	/// Returns the range of the data contained within the data source.  This goes through all the data every time and can be very expensive.  Subclasses should re-implement.
	virtual AMRange dataRange() const;

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



	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing this data source's custom parameters.  If you don't want to provide an editor widget, return 0.
	virtual QWidget* createEditorWidget() {	return 0; }


	/// Hint to indicate that this data source should be visible (in plots and graphical displays).  Users are free to toggle this visibility.
	virtual bool visibleInPlots() const { return visibleInPlots_; }
	/// Specify that this data source should be visible (in plots and graphical displays).  Users are free to toggle this visibility.
	virtual void setVisibleInPlots(bool isVisible);

	/// Hint to indicate that this data source should be hidden from users by default. (ie: it contains some programming internals). This means that users shouldn't see it, or be able to toggle its visibility.
	virtual bool hiddenFromUsers() const { return hiddenFromUsers_; }
	/// Specify that this data source should be hidden from users by default. (ie: it contains some programming internals). This means that users shouldn't see it, or be able to toggle its visibility.
	virtual void setHiddenFromUsers(bool isHidden = true) { if(isHidden == hiddenFromUsers_) return; hiddenFromUsers_ = isHidden; emitInfoChanged(); }


protected:

	/// Subclasses must call this when the data has changed (in value, or validity). It allows connected plots and analysis blocks to update / re-analyze. \c start and \c end specify the range of values that have changed. Use an invalid index (AMnDIndex()) for \c start to specify all data might have changed.
	void emitValuesChanged(const AMnDIndex& start = AMnDIndex(), const AMnDIndex& end = AMnDIndex()) { signalSource_->emitValuesChanged(start, end); }
	/// Subclasses must call this when the data has changed (in size). \c axisNumber specifies which axis changed, or -1 to indicate they all might have.  Changing rank (dimensionality) is not allowed.
	void emitSizeChanged(int axisNumber = -1) { signalSource_->emitSizeChanged(axisNumber); }
	/// Subclasses must call this when the descriptions of the axes have changed (units, description, etc.). \c axisNumber specifies which axis changed, or -1 to indicate they all might have.
	void emitAxisInfoChanged(int axisNumber = -1) { signalSource_->emitAxisInfoChanged(axisNumber); }
	/// This is emitted when the meta-info changes. (Right now, this only includes a data source's description(), and the visibleInPlots() and hiddenFromUsers() attributes. )
	void emitInfoChanged() { signalSource_->emitInfoChanged(); }
	/// Subclasses must call this when the state() of the data changes
	void emitStateChanged(int newDataState) { signalSource_->emitStateChanged(newDataState); }

	/// identifying name for this data source
	QString name_;
	/// Human-readable description for it
	QString description_;
	/// Units describing the dependent values return by value()
	QString units_;
	/// Set of observers
	QSet<void*> observers_;

	/// Hint to indicate that this data source should be visible (in plots and graphical displays).  Users are free to toggle this visibility.
	bool visibleInPlots_;
	/// Hint to indicate that this data source should be hidden from users by default. (ie: it contains some programming internals). This means that users shouldn't see it, or be able to toggle its visibility.
	bool hiddenFromUsers_;


private:
	/// QObject proxy for emitting signals. (This interface class can't emit directly, because it doesn't want to inherit QObject.)
	AMDataSourceSignalSource* signalSource_;

	/// Helper function to implement the base-class version of values(), when rank > 4.
	void valuesImplementationRecursive(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, AMnDIndex current, int dimension, double** outputValues) const;

};

Q_DECLARE_METATYPE(AMDataSource*)

#endif // AMDATASOURCE_H
