#ifndef AMMOCKDETECTOR_H
#define AMMOCKDETECTOR_H

#include "beamline/AMDetector.h"

/*!
  * A class which mocks a detector of 0 rank (ie. a single data point). The detector
  * can be configured to return values instantly, or dwell for a specified
  * acquisitionTime(). The detector only supports single-shot acquisition, and
  * cannot be placed in continuous mode.
  */
class AMMockDetector : public AMDetector
{
    Q_OBJECT
public:
	/// Creates an instance of a mock detector with the provided name, description
	/// and readMethod.
    explicit AMMockDetector(const QString& name,
	                        const QString& description,
	                        AMDetectorDefinitions::ReadMethod readMethod,
	                        QObject *parent = 0);

	/// Returns 0. The mock detector is currently only of rank 0.
	virtual int size(int /*axisNumber*/) const { return 0; }

	/// Returns false. The mock detector does not require power.
	virtual bool requiresPower() const { return false; }

	/// Returns false. The mock detector cannot be cancelled.
	virtual bool canCancel() const { return false; }

	/// Returns false. The mock detector cannot be cleared.
	virtual bool canClear() const { return false; }

	/// Returns false. The mock detector cannot continuous acquire.
	virtual bool canContinuousAcquire() const { return false; }

	/// The acquisition time used if the detector has a RequestRead or WaitRead
	/// readMethod.
	virtual double acquisitionTime() const;

	/// The tolerance which is used to detect if the acquisitionTime has elapsed
	/// when acquiring in dwell/wait mode.
	virtual double acquisitionTimeTolerance() const;

	/// Returns true if the read method which the detector was constructed with
	/// was WaitRead or RequestRead.
	virtual bool supportsSynchronizedDwell() const;

	/// Returns the key for this detector (for matching with synchronized dwell application)
	virtual QString synchronizedDwellKey() const;

	/// Takes no action, and returns false. The read mode of the mock detector
	/// cannot be altered from SingleRead.
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode /*readMode*/) { return false; }

	/// Returns the read method of the detector (SingleRead).
	virtual AMDetectorDefinitions::ReadMethod readMethod() const;

	/// Returns the read mode with which the detector was initialized.
	virtual AMDetectorDefinitions::ReadMode readMode() const;

	/// Provides a reading from the detector. The indexes argument is ignored as
	/// the mock detector is only of rank 0.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Provides a reading from the detector.
	virtual AMNumber singleReading() const;

	/// Writes a reading into the provided outputValues double. As the detector
	/// is only single point this is assumed to be an already initialized single
	/// double pointer.
	virtual bool data(double *outputValues) const;

	/// Returns 0, as the mock detector is only of rank 0.
	virtual AMDataSource* dataSource() const;

	/// Sets the acquisition time of the detector if the read method is WaitRead
	/// or RequestRead, and returns true. Otherwise takes no action and returns
	/// false.
	virtual bool setAcquisitionTime(double seconds);

	/// Sets the state of the detector to initialized and returns true.
	virtual bool initializeImplementation();

	/// If the readMethod is WaitRead or RequestRead the mock detector wait for
	/// acquisitionTime() before indicating that it has performed the acquisition,
	/// otherwise it does so instantly.
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);

	/// Sets the state to cleaned up.
	virtual bool cleanupImplementation();

	/// Helper funciton which is used to generate a random number between 1 and
	/// 1000. Used to return random results when data is requested of the detector.
	inline double generateRandomNumber() const;


signals:

public slots:
	void onRequestTimerTimedOut();
protected:

	double acquisitionTime_;
	double acquisitionTimeTolerance_;

	AMDetectorDefinitions::ReadMode readMode_;
	AMDetectorDefinitions::ReadMethod readMethod_;

};

#endif // AMMOCKDETECTOR_H
