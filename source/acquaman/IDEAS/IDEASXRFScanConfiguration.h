#ifndef IDEASXRFSCANCONFIGURATION_H
#define IDEASXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

class IDEASXRFScanConfiguration : public AMScanConfiguration
{
    Q_OBJECT

    Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)

    Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS XRF Scan Configuration")

public:
    Q_INVOKABLE explicit IDEASXRFScanConfiguration(QObject *parent = 0);
    /// Default constructor.
    IDEASXRFScanConfiguration(AMDetectorInfo detectorInfo, QObject *parent = 0);
    /// Destructor.
    virtual ~IDEASXRFScanConfiguration();

    /// Returns the detector info for the current detector.
    AMDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

    /// Returns a new instance of the scan configuration.
    virtual AMScanConfiguration *createCopy() const;

    /// Returns a new instance of the scan controller.
    virtual AMScanController *createController();

    /// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
    virtual QString description() const {
            return QString("XRF Free Run Scan");
    }

    /// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
    virtual QString detailedDescription() const;

    /// Returns the integration time.
    double integrationTime() const { return integrationTime_; }
    /// Returns the maximum energy in eV.
    double maximumEnergy() const { return maxEnergy_; }
    /// Returns the peaking time.
    double peakingTime() const { return peakingTime_; }

public slots:
    /// Sets the detector info to the given detector info.
    void setDetectorInfo(AMDetectorInfo info) { xrfDetectorInfo_ = info; /*setIntegrationTime(info.integrationTime()); setMaximumEnergy(info.maximumEnergy()); setPeakingTime(info.peakingTime());*/ }
    /// Sets the integration time.
    void setIntegrationTime(double time) { integrationTime_ = time; }
    /// Sets the maximum energy.
    void setMaximumEnergy(double energy) { maxEnergy_ = energy; }
    /// Sets the peaking time.
    void setPeakingTime(double time) { peakingTime_ = time; }

protected:
    /// Returns an AMDbObject pointer to the detector info.
    AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
    /// Empty function since it will never be called.
    void dbLoadXRFDetectorInfo(AMDbObject *) {}

    // Member variables.
    /// Detector info member variable.
    AMDetectorInfo xrfDetectorInfo_;

    /// The integration time.
    double integrationTime_;
    /// The maximum energy.  Stored in eV.
    double maxEnergy_;
    /// The peaking time.
    double peakingTime_;

signals:

public slots:

};

#endif // IDEASXRFSCANCONFIGURATION_H
