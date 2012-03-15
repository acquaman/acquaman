#ifndef REIXSXESSCANACTIONINFO_H
#define REIXSXESSCANACTIONINFO_H

#include "actions2/actions/AMScanControllerActionInfo.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

/// This is a subclass of AMScanControllerActionInfo for the purpose of registering REIXS XES Scan actions with the AMActionRegistry.
class REIXSXESScanActionInfo : public AMScanControllerActionInfo
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* scanConfig READ dbReadScanConfig WRITE dbLoadScanConfig)

public:
	/// Constructor: takes a REIXSXESScanConfiguration \c scanConfig, but still needs a default constructor for database loading.
	/*! If no \c scanConfig is provided, we create a valid REIXSXESScanConfiguration.*/
	Q_INVOKABLE REIXSXESScanActionInfo(REIXSXESScanConfiguration* scanConfig = new REIXSXESScanConfiguration(), QObject *parent = 0);
	/// Copy constructor. The base class takes care of making a copy of \c other's scan configuration.
	REIXSXESScanActionInfo(const REIXSXESScanActionInfo& other);
	/// Virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new REIXSXESScanActionInfo(*this); }


	/// The base class scanConfig() returns our REIXSXESScanConfiguration as an AMScanConfiguration. Here we return it for convenience as a REIXSXESScanConfiguration.
	REIXSXESScanConfiguration* xesConfig() { return qobject_cast<REIXSXESScanConfiguration*>(scanConfig_); }
	const REIXSXESScanConfiguration* xesConfig() const { return qobject_cast<const REIXSXESScanConfiguration*>(scanConfig_); }

signals:

public slots:

protected slots:
	/// Helper function to update the description when the configuration changes
	void onConfigurationChanged();

protected:
	/// Saves the scan config in the database, and also used to load the scan config from the db whenever we already have a valid one.
	AMDbObject* dbReadScanConfig();
	// Used to load the scan config from the database when we don't have a valid one yet.  This will never actually be called, since even the default-constructed instance returns a valid object for dbReadScanConfig(). Loading from the database will just call a dbLoad() on that valid scan configuration.
	void dbLoadScanConfig(AMDbObject* newObject);

};

#endif // REIXSXESSCANACTIONINFO_H
