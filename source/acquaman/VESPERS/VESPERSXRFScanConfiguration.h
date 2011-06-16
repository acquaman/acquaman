#ifndef VESPERSXRFSCANCONFIGURATION_H
#define VESPERSXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "dataman/VESPERS/XRFDetectorInfo.h"

class VESPERSXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)
	Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)
	Q_PROPERTY(double minimumEnergy READ minimumEnergy WRITE setMinimumEnergy)
	Q_PROPERTY(double maximumEnergy READ maximumEnergy WRITE setMaximumEnergy)
	Q_PROPERTY(double peakingTime READ peakingTime WRITE setPeakingTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XRF Scan Configuration")

public:
	/// Default constructor.
	Q_INVOKABLE explicit VESPERSXRFScanConfiguration(QObject *parent = 0);
	/// Convenience constructor.
	VESPERSXRFScanConfiguration(XRFDetectorInfo detectorInfo, QObject *parent = 0);
	/// Destructor.
	~VESPERSXRFScanConfiguration();

	/// Returns the detector info for the current detector.
	XRFDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the integration time.
	double integrationTime() const { return integrationTime_; }
	/// Returns the minimum energy in eV.
	double minimumEnergy() const { return minEnergy_; }
	/// Returns the maximum energy in eV.
	double maximumEnergy() const { return maxEnergy_; }
	/// Returns the peaking time.
	double peakingTime() const { return peakingTime_; }

public slots:
	/// Sets the detector info to the given detector info.
	void setDetectorInfo(XRFDetectorInfo info) { xrfDetectorInfo_ = info; }
	/// Sets the integration time.
	void setIntegrationTime(double time) { integrationTime_ = time; emit integrationTimeChanged(time); setModified(true); }
	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minEnergy_ = energy; emit minimumEnergyChanged(energy); setModified(true); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maxEnergy_ = energy; emit maximumEnergyChanged(energy); setModified(true); }
	/// Sets the peaking time.
	void setPeakingTime(double time) { peakingTime_ = time; emit peakingTimeChanged(time); setModified(true); }

signals:
	/// Notifier that the integration time has changed.
	void integrationTimeChanged(double);
	/// Notifer that the minimum energy has changed.
	void minimumEnergyChanged(double);
	/// Notifier that the maximum energy has changed.
	void maximumEnergyChanged(double);
	/// Notifier that the peaking time has changed.
	void peakingTimeChanged(double);

protected:
	/// Returns an AMDbObject pointer to the detector info.
	AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
	/// Empty function since it will never be called.
	void dbLoadXRFDetectorInfo(AMDbObject *) {}

	// Member variables.
	/// Detector info member variable.
	XRFDetectorInfo xrfDetectorInfo_;

	/// The integration time.
	double integrationTime_;
	/// The minimum energy.  Stored in eV.
	double minEnergy_;
	/// The maximum energy.  Stored in eV.
	double maxEnergy_;
	/// The peaking time.
	double peakingTime_;
};

#endif // VESPERSXRFSCANCONFIGURATION_H
