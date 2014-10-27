#ifndef SXRMB2DMAPSCANCONFIGURATION_H
#define SXRMB2DMAPSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

class SXRMB2DMapScanConfiguration : public AMStepScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB 2D Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE SXRMB2DMapScanConfiguration(QObject *parent = 0);
	/// Copy Constuctor.
	SXRMB2DMapScanConfiguration(const SXRMB2DMapScanConfiguration &original);
	/// Destructor.
	virtual ~SXRMB2DMapScanConfiguration() {}

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();
};

#endif // SXRMB2DMAPSCANCONFIGURATION_H
