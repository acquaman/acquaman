#ifndef VESPERSCCDDETECTOR_H
#define VESPERSCCDDETECTOR_H

#include "beamline/AMDetector.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "application/VESPERS/VESPERS.h"

class VESPERSCCDDetector : public AMDetector
{
	Q_OBJECT

public:
	/// Constructor.
	virtual ~VESPERSCCDDetector();
	VESPERSCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Returns the current acquisition dwell time which is only relevant for triggered (RequestRead) detectors
	virtual double acquisitionTime() const;
	/// Returns the elapsed time in seconds.
	double elapsedTime() { return elapsedTime_.elapsed()/1000.0; }

	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }
	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }
	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// The Vortex detectors share a triggering source  sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource();
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Vortex detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const { Q_UNUSED(indexes) return AMNumber(); }
	/// Returns a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual bool data(double *outputValues) const { outputValues[0] = 0; return true; }
	/// Returns a data source for viewing this detector's output
	virtual AMDataSource* dataSource() const { return 0; }


	/// Returns the current file path set for the detector.
	QString ccdFilePath() const;
	/// Returns the current file name for the detector.
	QString ccdFileBaseName() const;
	/// Returns the current number that is used for auto indexing of the file names.
	int ccdFileNumber() const;

	/// Returns the status control for the CCD detector.
	AMControl *statusControl() const { return acquisitionStatusControl_; }
	/// Returns the CCD file number control.
	AMControl *ccdFileNumberControl() const { return ccdFileNumberControl_; }

	/// Creates a newly created action that sets the file path for the detector.  Returns 0 if not connected.
	AMAction3 *createFilePathAction(const QString &path);
	/// Creates a newly created action that sets the file name for the detector.  Returns 0 if not connected.
	AMAction3 *createFileNameAction(const QString &name);
	/// Creates a newly created action that sets the number for auto-increment.  Returns 0 if not connected.
	AMAction3 *createFileNumberAction(int number);

signals:
	/// Notifier that the elapsed time has changed.  Passes the new value.
	void elapsedTimeChanged(double);
	/// Notifier that the CCD path has changed.
	void ccdPathChanged(const QString &);
	/// Notifier that the CCD file name has changed.
	void ccdNameChanged(const QString &);
	/// Notifier that the CCD number has changed.
	void ccdNumberChanged(int);

public slots:

	/// Sets the read mode for this detector (if possible, check with canContinuousAcquire)
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// Sets the CCD file path.
	void setCCDFilePath(QString path);
	/// Sets the CCD file name.
	void setCCDFileBaseName(QString name);
	/// Sets the CCD file number.
	void setCCDNumber(int number);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	virtual void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out.
	virtual void onControlsTimedOut();
	/// Handles changes in the status control.
	void onStatusControlChanged();
	/// Handles emitting the new elapsed time when it changes.
	void onElapsedTimeChanged();
	/// Handles the CCD path update.
	void onCCDFilePathChanged();
	/// Handles the CCD name update.
	void onCCDFileBaseNameChanged();
	/// Handles propogating the file number signal.
	void onCCDFileNumberChanged();

protected:
	/// A helper method that adds all of this classes controls to the allControls_ control set.
	void allControlsCreated();

	/// This function is called from the Initializing (intialization) state when the implementation should initialize the detector. Once the detector is initialized you must call setInitialized(), if initialization has failed you must call setInitializationRequired()
	virtual bool initializeImplementation();
	/// This function is called from the Acquiring (acquisition) state when the implementation should start the detector's acquisition. Once the detector is done acquiring you must call setAcquisitionSucceeded or setAcquisitionFailed
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	/// This function is called from the Cancelling (acquisition) state for detectors that support cancelling acquisitions. Once the detector has successfully cancelled the acquisition you must call setAcquisitionCancelled()
	virtual bool cancelAcquisitionImplementation();
	/// This function is called from the CleaningUp (cleanup) state when the implementation should cleanup the detector. Once the detector is cleaned up you must call setCleanedUp(), if clean up has failed you must call setCleanupRequired()
	virtual bool cleanupImplementation();

	/// The master set of controls
	AMControlSet *allControls_;

	/// The control for changes the acquisition state.
	AMPVControl *acquireControl_;
	/// Control for operation of the detector.  Status of acquiring, starting, stopping.
	AMReadOnlyPVControl *acquisitionStatusControl_;
	/// Control for the exposure time.
	AMPVControl *acquireTimeControl_;
	/// Control holding the path to the files.  Requires special reading and writing methods.
	AMPVControl *ccdFilePathControl_;
	/// Control holding the name of the file.  Requires special reading and writing methods.
	AMPVControl *ccdFileBaseNameControl_;
	/// Control holding the number of the file.
	AMPVControl *ccdFileNumberControl_;

	/// The QTime for keeping track of the elapsed time.
	QTime elapsedTime_;
	/// The timer used for updating the elapsed time.
	QTimer elapsedTimer_;
};

#endif // VESPERSCCDDETECTOR_H
