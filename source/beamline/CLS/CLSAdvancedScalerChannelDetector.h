#ifndef CLSADVANCEDSCALERCHANNELDETECTOR_H
#define CLSADVANCEDSCALERCHANNELDETECTOR_H

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"

class CLSAdvancedScalerChannelDetector : public CLSBasicScalerChannelDetector
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as the scaler object pointer and the channel index to use (index 0 - 31 for SIS3820)
 	virtual ~CLSAdvancedScalerChannelDetector();
	CLSAdvancedScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent = 0);

	/// Advanced scaler channels are designed to implement continuous acquire.
	virtual bool canContinuousAcquire() const { return true; }

	/// Returns SingleRead or ContinuousRead depending on the current mode
	virtual AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

	/// Returns true if the last continuous reading was valid and the outputValues are passed back in \c outputValues.
	virtual bool lastContinuousReading(double *outputValues) const;
	virtual int lastContinuousSize() const;

	/// Fills the (hopefully) valid pointer to a single double with our current value
	virtual bool data(double *outputValues) const;

	/// Returns boolean indicating that this particular implementation of AMDetector does not support dark current correction.
	virtual bool canDoDarkCurrentCorrection() const { return false; }

public slots:
	/// Note that this will change the read mode for the entire scaler, not just this channel. The read mode can be changed to either SingleRead or ContinuousRead. Requesting to change to the same readMode returns true and the readModeChanged() signal is immediately emitted
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

protected slots:
	/// Handles watching signals from the scaler while we're switching modes
	void onModeSwitchSignal();

	/// Updates the detector status from the scaler's scanning status
	virtual void onScalerScanningChanged(bool isScanning);
	virtual void onReadingChanged();

	void onScansPerBufferChanged(int scansPerBuffer);
	void onTotalScansChanged(int totalScans);

	bool triggerChannelAcquisition();

	void onScalerConnectedConfirmReadMode(bool connected);

protected:
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);


protected:
	AMDetectorDefinitions::ReadMode readMode_;

	bool switchingReadModes_;

	QVector<double> continuousData_;
	int continuousSize_;
};

#endif // CLSADVANCEDSCALERCHANNELDETECTOR_H
