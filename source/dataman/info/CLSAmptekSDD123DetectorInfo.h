#ifndef CLSAMPTEKSDD123DETECTORINFO_H
#define CLSAMPTEKSDD123DETECTORINFO_H

#include "dataman/info/AMDetectorInfo.h"

/// This class contains the run-time configuration parameters for the Amptek SDD 123 detectors
class CLSAmptekSDD123DetectorInfo : public AMDetectorInfo
{
Q_OBJECT


public:
	/// Default constructor
	Q_INVOKABLE CLSAmptekSDD123DetectorInfo(const QString &name = "amptekSDD123", const QString &description = "Amptek SDD Detector", QObject *parent = 0);
	/// Copy constructor
	CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original);

	/// Implement assignment operator
	CLSAmptekSDD123DetectorInfo& operator =(const CLSAmptekSDD123DetectorInfo &other);
};

#endif // CLSAMPTEKSDD123DETECTORINFO_H
