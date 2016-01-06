#ifndef AM1DCONTROLDETECTOREMULATOR_H
#define AM1DCONTROLDETECTOREMULATOR_H

#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"

/// This class takes an AMControl that is expected to represent a waveform record (ie. multi-valued) and expose it as an AMDetector.
class AM1DControlDetectorEmulator : public AMDetector
{
	Q_OBJECT

public:
	/// Constructor.  Takes a name, description, and AMControl as arguments.
	AM1DControlDetectorEmulator(const QString &name, const QString &description, int size, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod,  QObject *parent = 0);
	/// Destructor.
	virtual ~AM1DControlDetectorEmulator(){}

	/// Returns the size of the waveform.  This should always be passed an axisNumber of 0.
	virtual int size(int axisNumber) const;
	/// The basic controls don't require additional power.
	virtual bool requiresPower() const { return false; }
	/// Cancelling is not implemented for controls.
	virtual bool canCancel() const { return false; }
	/// Clearing is not implemented for controls.
	virtual bool canClear() const { return false; }

	/// Basic controls cannot acquire continously.
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns -1 because these controls don't have access to this information.
	virtual double acquisitionTime() const { return -1; }
	/// Returns the acquisition time tolerance.  Retunrs -1 because these are not part of these detectors.
	virtual double acquisitionTimeTolerance() const { return -1; }

	/// These are not configured to use the synchronized dwell time.
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// There is no key if it can't support synchronization.
	virtual QString synchronizedDwellKey() const { return ""; }

	/// No triggering source for these detectors.
	virtual bool sharesDetectorTriggerSource() const { return false; }
	/// Returns a null trigger source as it doesn't support this.
	virtual AMDetectorTriggerSource *detectorTriggerSource() { return 0; }

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return readMethod_; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the control value
	virtual AMNumber singleReading() const;

	/// Fills the given double pointer with our current value
	virtual bool data(double *outputValues) const;

	virtual AMAction3* createSetAcquisitionTimeAction(double seconds) { Q_UNUSED(seconds); return 0; }
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode);

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return 0; }

	/// Returns the flag to determine if this detector should be accessed through the lastFloatingPointValues() call. The normal is lastIntegerValues()
	bool accessAsDouble() const { return accessAsDouble_; }

public slots:
	/// Does nothing and returns false
	virtual bool setAcquisitionTime(double seconds) { Q_UNUSED(seconds); return false; }

	/// The read mode cannot be changed for basic controls
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode) { Q_UNUSED(readMode); return false; }

	/// Controls do not support clearing
	virtual bool clear() { return false; }

	/// Sets the flag to determine if this detector should be accessed through the lastFloatingPointValues() call. The normal is lastIntegerValues()
	void setAccessAsDouble(bool accessAsDouble) { accessAsDouble_ = accessAsDouble; }

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();

	/// Handles changes to the control we're monitoring
	void onControlValueChanged(double value);
	/// Handles changes to the status control (if we have one)
	void onStatusControlValueChanged(double value);

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cleanupImplementation();

	/// Checks all of the scaler settings to make sure we're ready for an acquisition
	void checkReadyForAcquisition();

protected:
	/// Holds the read method we want to use
	AMDetectorDefinitions::ReadMethod readMethod_;

	/// Holds the pointer to the control we're using
	AMReadOnlyPVControl *control_;
	/// Holds the status control for cases where we're using request read
	AMReadOnlyPVControl *statusControl_;
	/// The master set of controls
	AMControlSet *allControls_;

	bool waitingForNewData_;
	bool waitingForStatusChange_;

	double statusAcquiringValue_;
	double statusNotAcquiringValue_;

	/// A flag to determine if this detector should be accessed through the lastFloatingPointValues() call. The normal is lastIntegerValues()
	bool accessAsDouble_;
};

#endif // AM1DCONTROLDETECTOREMULATOR_H
