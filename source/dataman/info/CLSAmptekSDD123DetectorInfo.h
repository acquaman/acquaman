#ifndef CLSAMPTEKSDD123DETECTORINFO_H
#define CLSAMPTEKSDD123DETECTORINFO_H

#include "dataman/info/AMBasicXRFDetectorInfo.h"

/// This class contains the run-time configuration parameters for the Amptek SDD 123 detectors
class CLSAmptekSDD123DetectorInfo : public AMBasicXRFDetectorInfo
{
Q_OBJECT

Q_PROPERTY(double detectorTemperature READ detectorTemperature WRITE setDetectorTemperature)

Q_CLASSINFO("AMDbObject_Attributes", "description=Amptek SDD123 Detector")

public:
	/// Default constructor
	Q_INVOKABLE CLSAmptekSDD123DetectorInfo(const QString &name = "amptekSDD123", const QString &description = "Amptek SDD Detector", QObject *parent = 0);
	/// Copy constructor
	CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMDetectorInfo* toNewInfo() const;

	/// Implement assignment operator
	CLSAmptekSDD123DetectorInfo& operator =(const CLSAmptekSDD123DetectorInfo &other);

	double detectorTemperature() const;

public slots:
	void setDetectorTemperature(double detectorTemperature);

protected:
	/// The detector temperature
	double detectorTemperature_;
};

#endif // CLSAMPTEKSDD123DETECTORINFO_H
