#ifndef SXRMBFOURELEMENTVORTEXDETECTOR_H
#define SXRMBFOURELEMENTVORTEXDETECTOR_H

#include "beamline/AMXRFDetector.h"
#include "application/SXRMB/SXRMB.h"

class AMAction3;

/// Implementation of AMXRFDetector for the four element vortex detector used on the SXRMB beamline.
class SXRMBFourElementVortexDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.  Requires the name and description of the detector.  It builds all PV's and connects them accordingly.
	SXRMBFourElementVortexDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMBFourElementVortexDetector();

	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const;
	/// Returns the type of the detector
	virtual int type() { return SXRMB::FourElementDetector; }
	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the maximum energy for this detector.  Result returned in keV.
	double maximumEnergy() const { return maximumEnergyControl_->value(); }
	/// Returns the peaking time for this detector.  Result returned in us.
	double peakingTime() const { return peakingTimeControl_->value(); }

	/// Returns a newly created action (possibly list of actions) to perform the detector initialization
	/// which will try to set dxp1606-B10-02:ReadAll.SCAN and dxp1606-B10-02:ReadDXPs.SCAN to "Passive (0)" mode
	/// set dxp1606-B10-02:StatusAll.SCAN to the fastest rate (9: 0.1 s)
	virtual AMAction3* createInitializationActions();
	/// Returns a newly created action (possibly list of actions) to perfrom the detector cleanup
	/// which will try to restore dxp1606-B10-02:ReadAll.SCAN and dxp1606-B10-02:ReadDXPs.SCAN to the original mode
	/// set dxp1606-B10-02:StatusAll.SCAN to the original rate
	virtual AMAction3* createCleanupActions();

signals:
	/// Notifier that the maximum energy has changed.  Value passed in keV.
	void maximumEnergyChanged(double);
	/// Notifier that the peaking time has changed.  Value passed in us.
	void peakingTimeChanged(double);

public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }

	/// Sets the maximum energy.  Expects \param energy in keV.
	void setMaximumEnergy(double energy);
	/// Sets the peaking time.  Expects \param time in us.
	void setPeakingTime(double time);

protected slots:
	/// Handles changing the scale for the raw spectra sources when the maximum energy changes.
	void onMaximumEnergyChanged(double newMaximum);
	/// Handles updating the acquisition state of the detector.
	void updateAcquisitionState();
	/// Handles setting the acquisition succeeded state by waiting on all the spectra.
	void onDataChanged();

protected:
	/// The maximum energy control.
	AMPVControl *maximumEnergyControl_;
	/// The peaking time control.
	AMPVControl *peakingTimeControl_;

	/// the MCA update control
	AMPVControl *mcaRatePVControl_;
	/// the DXP update control
	AMPVControl *dxpRatePVControl_;
	/// the Status update control
	AMPVControl *statusRatePVControl_;

	/// Data ready flag that helps determine when the detector finished its acquisition.
	bool dataReady_;
	/// Counter to know how many of the spectra sources have updated their values since acquisition started.
	int dataReadyCounter_;};

#endif // SXRMBFOURELEMENTVORTEXDETECTOR_H
