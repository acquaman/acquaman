#ifndef PGMPICOAMMETER_H
#define PGMPICOAMMETER_H

#include "beamline/AMDetector.h"

#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

/// Pico ammeter detector class for PGM.
class PGMPicoAmmeter : public AMDetector
{
	Q_OBJECT

public:
	/// Constructor.
	PGMPicoAmmeter(const QString &name, const QString &description, const QString &pvName, QObject *parent = 0);
	/// Destructor.
	virtual ~PGMPicoAmmeter(){}

	/// Returns whether a detector requires power (high voltage, likely) to operate. Every detector subclass need to implement this function.
	virtual bool requiresPower() const { return false; }

	/// Returns whether a detector can (generally) have its acquisition cancelled
	virtual bool canCancel() const { return false; }
	/// Returns whether a detector can (generally) be cleared
	virtual bool canClear() const { return false; }
	/// Returns whether a detector is capable of running in continuous mode
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the current acquisition dwell time which is only relevant for triggered (RequestRead) detectors
	virtual double acquisitionTime() const;
	/// Returns the acquisition time tolerance.  This is defined by subclasses because this limitation will likely be detector specific.
	virtual double acquisitionTimeTolerance() const { return 0.01; }

	/// Returns whether or not this detector can be coordinated with a synchronized dwell system
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns whether or not this detector is currently interfaced with a synchronized dwell system by querying the AMBeamline class. You may reimplement if you wish.
	virtual bool currentlySynchronizedDwell() const;
	/// Returns the key for this detector (for matching with synchronized dwell application)
	virtual QString synchronizedDwellKey() const;

	/// Returns whether to not this detector shares a triggering source, such as synchronized dwell time or the main scaler trigger. Default implementation returns false.
	virtual bool sharesDetectorTriggerSource() const { return true; }
	/// Returns the trigger source for this detector. Default implementation returns a NULL pointer.
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the dwell time source for this detector. Default implementation returns a NULL pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns the read method for this detector
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::ImmediateRead; }
	/// Returns the read mode for this detector
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns a total single reading for the detector (the value for 0D, possibliy a sum for 1D). If this is not possible, the default implementation is sufficient to return an invalid AMNumber.
	virtual AMNumber singleReading() const;

	/// Fills the given double pointer with the current detector data in row-major order (first axis varies slowest).  Memory must be preallocated to the size of the detector data.
	virtual bool data(double *outputValues) const;
	/// Returns a data source for viewing this detector's output
	virtual AMDataSource *dataSource() const;

signals:
	/// Notifier when the value changes.
	void valueChanged(double);

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);
	/// Sets the read mode for this detector (if possible, check with canContinuousAcquire)
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode) { Q_UNUSED(readMode); return true; }

protected slots:
	/// Sets the units when the control connnects and if the units change.
	void updateUnits();
	/// Initializes the detector once it connects.
	void onConnectedChanged(bool connected);

protected:
	/// This function is called from the Initializing (intialization) state when the implementation should initialize the detector. Once the detector is initialized you must call setInitialized(), if initialization has failed you must call setInitializationRequired()
	virtual bool initializeImplementation();
	/// This function is called from the Acquiring (acquisition) state when the implementation should start the detector's acquisition. Once the detector is done acquiring you must call setAcquisitionSucceeded or setAcquisitionFailed
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	/// This function is called from the CleaningUp (cleanup) state when the implementation should cleanup the detector. Once the detector is cleaned up you must call setCleanedUp(), if clean up has failed you must call setCleanupRequired()
	virtual bool cleanupImplementation();

	/// The read only pv control that is the data for the detector.
	AMReadOnlyPVControl *control_;
	/// The data source for encapsulating the control.
	AM0DProcessVariableDataSource *dataSource_;
};

#endif // PGMPICOAMMETER_H
