#ifndef CLSJJSLITSCANCONFIGURATION_H
#define CLSJJSLITSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitScanConfiguration : public AMGenericStepScanConfiguration
{
    Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=JJ Slit Optimization Configuration")

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitScanConfiguration(const CLSJJSlitScanConfiguration &original);
	/// Destructor.
	virtual ~CLSJJSlitScanConfiguration();

	/// Returns a newly-created copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created scan controller appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController();
	/// Returns a newly-created view that is appropriate for viewing this kind of scan configuration.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique.
	virtual QString technique() const { return "JJ Slit Scan"; }
	/// Returns the description.
	virtual QString description() const { return "JJ Slit Scan"; }
	/// Returns the detailed description.
	virtual QString detailedDescription() const { return "A step scan for a given JJ slit"; }
};

#endif // CLSJJSLITSCANCONFIGURATION_H
