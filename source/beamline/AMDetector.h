#ifndef AMDETECTOR_H
#define AMDETECTOR_H

#include <QObject>

#include "dataman/AMMeasurementInfo.h"
#include "dataman/info/AMDetectorInfo.h"
#include "actions3/AMAction3.h"
#include "dataman/datasource/AMDataSource.h"

class AMDetector : public QObject
{
Q_OBJECT

public:
	/// Default constructor. \c name is a unique programmer name to access this detector with. \c description is a human-readable version
	AMDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// One feature of a detector is that it can create a snapshot of its current state and pass it on as an AMDetectorInfo.
	AMDetectorInfo toInfo() const;

	/// Access the programmer unique name
	QString name() const { return objectName(); }
	/// Access a human-readable description
	QString description() const { return description_; }
	/// Access the describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 0; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const = 0;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return QList<AMAxisInfo>(); }

	/// Returns (or casts) this AMDetector as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo();

	/// Returns the current connected state (whether the detector has a connection to its constituent elements)
	bool isConnected() const { return connected_; }
	/// Returns the current ready for acquisition state (whether the detector can acquire data right now)
	bool isReadyForAcquisition() const { return readyForAcquisition_; }
	/// Returns the current powered state (whether the detector has it's high voltage on). Also see requiresPower().
	bool isPowered() const { return powered_; }
	/// Returns whether a detector requires power (high voltage, likely) to operate. Every detector subclass need to implement this function.
	virtual bool requiresPower() const = 0;

	/// Returns true if the detector is currently acquiring.
	virtual bool isAcquiring() const = 0;
	/// Returns whether a detector can (generally) have its acquisition cancelled
	virtual bool canCancel() const = 0;
	/// Returns whether a detector can (generally) be cleared
	virtual bool canClear() const = 0;

	/// Returns whether a detector is capable of running in continuous mode
	virtual bool canContinuousAcquire() const = 0;

	/// Returns the current acquisition dwell time which is only relevant for triggered (RequestRead) detectors
	virtual double acquisitionTime() const = 0;

	/// Returns whether or not this detector can be coordinated with a synchronized dwell system
	virtual bool supportsSynchronizedDwell() const = 0;
	/// Returns whether or not this detector is currently interfaced with a synchronized dwell system
	virtual bool currentlySynchronizedDwell() const = 0;

	/// Returns the read method for this detector
	virtual AMDetectorDefinitions::ReadMethod readMethod() const = 0;
	/// Returns the read mode for this detector
	virtual AMDetectorDefinitions::ReadMode readMode() const = 0;

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const = 0;

	/// Copies a block of values from \c indexStart to \c indexEnd, into \c outputValues. 0D is an convenience call to reading, 1D copies a slice, 2D copies a plane.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! The base-class implementation simply calls reading() repeatedly, so you should absolutely re-implement this for better performance.

	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

\code
int outputSize = indexStart.totalPointsTo(indexEnd);
\endcode
*/
	// FINISH IMPLEMENTING THESE
	virtual bool reading0D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	virtual bool reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	/// Convenience call to access the three previous calls from a common interface. Pass 0, 1, or 2 into the dimensionality variable.
	bool readingND(int dimensionality, const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	// FLESH THIS ONE OUT
	/// Returns the data from the last continuous reading in the outputValues
	virtual bool lastContinuousReading(double *outputValues) const = 0;

	/// Returns a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual const double* data() const = 0;

	/// Returns a newly created action (possibly list of actions) to perform the detector initialization
	virtual AMAction3* createInitializationActions() = 0;

	/// Returns a newly created action to set the acquisition time on this detector
	virtual AMAction3* createSetAcquisitionTimeAction(double seconds) = 0;

	/// Returns a newly created action (possibly list of actions) to perfrom the detector cleanup
	virtual AMAction3* createCleanupActions() = 0;

	/// Returns a data source for viewing this detector's output
	virtual AMDataSource* dataSource() const = 0;

public slots:
	/// Initializes the detector (prepares it for acquisition)
	virtual bool initialize() = 0;

	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds) = 0;

	/// Sets the read mode for this detector (if possible, check with canContinuousAcquire)
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode) = 0;

	/// For triggered detectors (RequestRead), starts the acquisition process. For other detectors (ImmediateRead, WaitRead), does nothing. Returns if the acquistion started successfully. Pass in the type of read you wish to trigger.
	virtual bool acquire(AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead) = 0;

	/// For triggered detectors (RequestRead), cancels the read if this is possible. Returns whether or not the cancelation was possible.
	virtual bool cancelAcquisition() = 0;

	/// For clearable detectors, clears the current data. Returns whether the clear was possible.
	virtual bool clear() = 0;

	/// Cleans up the detector after its use is complete
	virtual bool cleanup() = 0;

signals:
	/// Indicates that the detector's constituent elements are connected (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
	/// Indicates that the detector is ready to acquire data (each detector sub class can define this however makes most sense)
	void readyForAcquisition(bool isReadyForAcquisition);
	/// Indicates that the detector is currently powered (has it's high voltage on). Also see requiresPower().
	void powered(bool isPowered);

	/// Emit this when the acquisition process finishes and pass whether it was successful or not
	void acquisitionFinished(bool succeeded);

protected:
	/// Internal class trigger for setting the connected state. All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setConnected(bool isConnected);
	/// Internal class trigger for setting the ready for acquisition state. All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setReadyForAcquisition(bool isReadyForAcquisition);
	/// Internal class trigger for setting the powered state. Also see requiresPower(). All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setPowered(bool isPowered);

	/// Helper function for checking rank and bounds on readingXD functions
	bool checkValid(const AMnDIndex &startIndex, const AMnDIndex &endIndex) const;

protected:
	/// A human-readable description
	QString description_;
	/// Units describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units_;

private:
	/// Internal state for connection, which referes to the constituent elements of the detector (likely AMControls) all being connected. Use setConnected(bool) to change so signals are emitted.
	bool connected_;
	/// Internal state for ready for acquisition, which refers to the detector being ready to acquire data at the moment. Use setReadyForAcquisition(bool) to change so signals are emitted.
	bool readyForAcquisition_;
	/// Internal state for powered, which refers to whether the detector is currently powered (many detectors require a high voltage to operate)
	bool powered_;
};

#endif // AMDETECTOR_H
