#ifndef VESPERSTIMESCANCONFIGURATION_H
#define VESPERSTIMESCANCONFIGURATION_H

#include "acquaman/AMTimedRegionScanConfiguration.h"

class VESPERSTimeScanConfiguration : public AMTimedRegionScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Timed Scan Configuration")

public:
	Q_INVOKABLE VESPERSTimeScanConfiguration(QObject *parent = 0);
	VESPERSTimeScanConfiguration(const VESPERSTimeScanConfiguration &original);
	virtual ~VESPERSTimeScanConfiguration(){}

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

};

#endif // VESPERSTIMESCANCONFIGURATION_H
