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


#ifndef AMSCANACTIONINFO_H
#define AMSCANACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMScanConfiguration;
class AMScan;

#define AMSCANACTIONINFO_CANNOT_LOAD_FROM_DB 104000
#define AMSCANACTIONINFO_DB_OBJECT_NOT_A_SCAN 104001
#define AMSCANACTIONINFO_SCAN_HAS_NO_CONFIGURATION 104002
#define AMSCANACTIONINFO_CREATE_CONFIGURATION_COPY_FAILED 104003

class AMScanActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* config READ dbGetConfig WRITE dbLoadConfig)
	Q_PROPERTY(int scanID READ scanID WRITE setScanID)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Scan Action")

public:
	Q_INVOKABLE AMScanActionInfo(QObject *parent = 0);
	/// Constructor.  Takes an AMScanConfiguration \param config and builds a scan action around it.  This will create a scan controller which can be controlled from within the workflow.
	Q_INVOKABLE AMScanActionInfo(AMScanConfiguration *config, const QString& iconFileName = ":/spectrum.png", QObject *parent = 0);
	/// Copy constructor.  Makes a copy of \param other's scan configuration but does not copy the scan ID
	AMScanActionInfo(const AMScanActionInfo &other);
	/// Destructor.
	virtual ~AMScanActionInfo();

	/// Creates a copy of this action info.
	virtual AMActionInfo3 *createCopy() const { return new AMScanActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Scan Action"; }

	// New public functions
	//////////////////////////

	/// Returns true if it has a valid config()
	bool isValid() const { return config_ != 0; }
	/// Access the scan configuration.  Returns a constant reference.
	const AMScanConfiguration* config() const;
	/// Access the scan configuration.  Returns a modifiable reference.  If the info does not have a valid pointer then it will attempt to retrieve one from the database.
	AMScanConfiguration* config();

	/// Returns the id of the scan that this action is associated with.
	int scanID() const { return scanID_; }
	/// Sets the id of the scan this action is associated with.
	void setScanID(int id) { scanID_ = id; }

	/// Overrides the warnings string to check warnings from the scan configuration
	virtual QString dbLoadWarnings() const;

protected:
	/// Helper to save to db
	AMDbObject* dbGetConfig() const;
	/// Helper to load from db
	void dbLoadConfig(AMDbObject *newConfig);

protected slots:
	/// Responds to changes to the config, updates that scan action info
	void onConfigChanged();

protected:
	/// The pointer holding the configuration that everything is based on.
	AMScanConfiguration *config_;
	/// The id for the scan for fast database loading.
	int scanID_;

private:
	/// Internal helper method that grabs a configuration from the database.  Returns 0 if it fails.
	AMScanConfiguration *getConfigurationFromDb() const;
};

#endif // AMSCANACTIONINFO_H
