#ifndef REIXSXESSCANACTIONINFO_H
#define REIXSXESSCANACTIONINFO_H

#include "actions2/actions/AMScanControllerActionInfo.h"

class REIXSXESScanConfiguration;

/// This is a subclass of AMScanControllerActionInfo for the purpose of registering REIXS XES Scan actions with the AMActionRegistry.
class REIXSXESScanActionInfo : public AMScanControllerActionInfo
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* scanConfig READ dbReadScanConfig WRITE dbLoadScanConfig)

public:
	/// Constructor: takes a REIXSXESScanConfiguration \c scanConfig, but still needs a default constructor for database loading
	Q_INVOKABLE REIXSXESScanActionInfo(REIXSXESScanConfiguration* scanConfig = 0, QObject *parent = 0);
	/// Copy constructor. The base class takes care of making a copy of \c other's scan configuration, if there is one.
	REIXSXESScanActionInfo(const REIXSXESScanActionInfo& other) : AMScanControllerActionInfo(other) {}
	/// Virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new REIXSXESScanActionInfo(*this); }



	/// Convenience function to extract the detector energy from the config. Returns 0 if this was a default-constructed actionInfo with no valid configuration.
	double configCenterEV() const;
	/// Convenience function to extract the defocus distance from the config. Returns 0 if this was a default-constructed actionInfo with no valid configuration.
	double configDefocusMm() const;
	/// Convenience function to extract the tilt offset from the config. Returns 0 if this was a default-constructed actionInfo with no valid configuration.
	double configTiltOffset() const;

signals:

public slots:

	/// Convenience function to set the detector energy and update the descriptions
	void setConfigCenterEV(double eV);
	/// Convenience function to set the defocus offset and update the descriptions
	void setConfigDefocusMm(double mm);
	/// Convience function to set the detector tilt offset and update the descriptions
	void setConfigTiltOffset(double degrees);

protected:
	/// Saves the scan config in the database, and also used to load the scan config from the db whenever we already have a valid one.
	AMDbObject* dbReadScanConfig();
	/// Used to load the scan config from the database when we don't have a valid one yet.
	void dbLoadScanConfig(AMDbObject* newObject);

	/// The base class scanConfig() returns our REIXSXESScanConfiguration as an AMScanConfiguration. Here we return it for convenience as a REIXSXESScanConfiguration.
	REIXSXESScanConfiguration* xesConfig();
	const REIXSXESScanConfiguration* xesConfig() const;
};

#endif // REIXSXESSCANACTIONINFO_H
