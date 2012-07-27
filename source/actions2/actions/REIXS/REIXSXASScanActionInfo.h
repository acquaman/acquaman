/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef REIXSXASSCANACTIONINFO_H
#define REIXSXASSCANACTIONINFO_H

#include "actions2/actions/AMScanControllerActionInfo.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

/// This is a subclass of AMScanControllerActionInfo for the purpose of registering REIXS XAS Scan actions with the AMActionRegistry.
class REIXSXASScanActionInfo : public AMScanControllerActionInfo
{
    Q_OBJECT
	Q_PROPERTY(AMDbObject* scanConfig READ dbReadScanConfig WRITE dbLoadScanConfig)

public:
	/// Constructor: takes a REIXSXASScanConfiguration \c scanConfig, but still needs a default constructor for database loading.
	/*! If no \c scanConfig is provided, we create a valid REIXSXASScanConfiguration.*/
	Q_INVOKABLE REIXSXASScanActionInfo(REIXSXASScanConfiguration* scanConfig = new REIXSXASScanConfiguration(), QObject *parent = 0);
	/// Copy constructor. The base class takes care of making a copy of \c other's scan configuration.
	REIXSXASScanActionInfo(const REIXSXASScanActionInfo& other);
	/// Virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new REIXSXASScanActionInfo(*this); }


	/// The base class scanConfig() returns our REIXSXASScanConfiguration as an AMScanConfiguration. Here we return it for convenience as a REIXSXASScanConfiguration.
	REIXSXASScanConfiguration* XASConfig() { return qobject_cast<REIXSXASScanConfiguration*>(scanConfig_); }
	const REIXSXASScanConfiguration* XASConfig() const { return qobject_cast<const REIXSXASScanConfiguration*>(scanConfig_); }

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

#endif // REIXSXASSCANACTIONINFO_H
