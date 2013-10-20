#ifndef SGMXASSCANCONFIGURATION2013_H
#define SGMXASSCANCONFIGURATION2013_H

#include "acquaman/AMXASScanConfiguration.h"
#include "SGMScanConfiguration2013.h"

class SGMXASScanConfiguration2013 : public AMXASScanConfiguration, public SGMScanConfiguration2013
{
Q_OBJECT

Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM XAS Scan Configuration 2013")

public:
	/// Constructor
	Q_INVOKABLE SGMXASScanConfiguration2013(QObject *parent = 0);
	/// Copy Constructor
	SGMXASScanConfiguration2013(const SGMXASScanConfiguration2013 &original);

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }
};

#endif // SGMXASSCANCONFIGURATION2013_H
