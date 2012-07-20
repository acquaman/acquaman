#ifndef REIXSXASSCANCONFIGURATION_H
#define REIXSXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

/// Scan configuration for XAS scans on REIXS
class REIXSXASScanConfiguration : public AMXASScanConfiguration
{
    Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "description=REIXS XAS Scan Configuration")

public:
	Q_INVOKABLE explicit REIXSXASScanConfiguration(QObject *parent = 0);
	virtual ~REIXSXASScanConfiguration();

	/// Copy constructor
	REIXSXASScanConfiguration(const REIXSXASScanConfiguration& other);

	/// Creates a fresh copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;

	/// Creates a scan controller capable of running this configuration
	virtual AMScanController* createController();
	/// Creates a view to edit the scan configuration
	virtual AMScanConfigurationView* createView();


	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo regionControlInfo() const { return AMControlInfo(); }
	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo timeControlInfo() const { return AMControlInfo(); }

signals:

public slots:

protected:



};

#endif // REIXSXASSCANCONFIGURATION_H
