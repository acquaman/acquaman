#ifndef IDEASXASSCANCONFIGURATION_H
#define IDEASXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

class IDEASXASScanConfiguration : public AMXASScanConfiguration
{
Q_OBJECT

Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS XAS Scan Configuration")

public:
	/// Constructor
	Q_INVOKABLE IDEASXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor
	IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original);
	virtual ~IDEASXASScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }
};

#endif // IDEASXASSCANCONFIGURATION_H
