#ifndef AMSTEPSCANCONFIGURATION_H
#define AMSTEPSCANCONFIGURATION_H

#include "AMScanConfiguration.h"

/// This is the generic scan configuration for all step based scans.
class AMStepScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObjectList scanAxes READ dbReadScanAxes WRITE dbLoadScanAxes)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Base configuration for step axis scans.")

public:
	/// Constructor.  Builds a generic scan configuration that handles all evenly spaced, n-dimensional step scans.
	AMStepScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMStepScanConfiguration(const AMStepScanConfiguration &original);
	/// Destructor.
	virtual ~AMStepScanConfiguration();

protected:
	/// Returns a list of the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadScanAxes();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects.
	void dbLoadScanAxes(const AMDbObjectList& newScanAxes);
};

#endif // AMSTEPSCANCONFIGURATION_H
