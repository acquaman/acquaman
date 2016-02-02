/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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

/*!
  * A class representing the persistant data to be stored relating to a scan action.
  * Allows for the decoupling of the information which describes a scan action from
  * the actual actions required to perform it.
  * Stores a reference to the scan's configuration and id as well as the standard
  * information contained within AMActionInfo3 (Short description, long description,
  * icon file name, expected duration and whether the scan action is copyable.
  */
class AMScanActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configuration READ dbGetConfig WRITE dbLoadConfig)
	Q_PROPERTY(int scanID READ scanID WRITE setScanID)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Scan Action")

public:
	/*!
	  * Default constructor for a scan action info. Creates a scan action info with
	  * no scan config and a scan id of -1.
	  */
	Q_INVOKABLE AMScanActionInfo(QObject *parent = 0);

	/*!
	  * Creates an instace of a scan action info with the provided configuration
	  * a short description pulled from the scan configuration's userScanName and
	  * a long description pulled from the scan configuration's description. The
	  * scan id will be set to -1.
	  */
	Q_INVOKABLE AMScanActionInfo(AMScanConfiguration *configuration, const QString& iconFileName = ":/spectrum.png", QObject *parent = 0);

	/*!
	  * Copy constructor. Creates a copy of the configuration contained within
	  * other and sets the scan id of the created copy to -1.
	  */
	AMScanActionInfo(const AMScanActionInfo &other);

	/*!
	  * Default destructor for the AMScanActionInfo. Performs no action.
	  */
	virtual ~AMScanActionInfo();

	/*!
	  * Creates a new scan action info from this scan action info and returns a
	  * reference to it. The copied scan action info is considered to be new with
	  * regard to the database (ie. saving the copied scan action info will insert
	  * a new record).
	  */
	virtual AMActionInfo3 *createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/*!
	  * A description of the type of the action.
	  */
	virtual QString typeDescription() const { return "Scan Action"; }

	// New public functions
	//////////////////////////

	/*!
	  * Whether or not the scan action has a valid config.
	  */
	bool isValid() const { return config_ != 0; }

	/*!
	  * Read-only access to the scan action's scan config.
	  */
	const AMScanConfiguration* configuration() const;

	/*!
	  * The scan action's scan config. If no config is currently stored in the
	  * scan action info, then an attempt to load it from the database is made.
	  */
	AMScanConfiguration* configuration();

	/*!
	  * The id of the scan which the scan action info is associated with.
	  */
	int scanID() const { return scanID_; }

	/*!
	  * Sets the id of the scan which the scan action info is associated with.
	  * \param id ~ The new id of the scan which the scan action info will be
	  * associated with.
	  */
	void setScanID(int id) { scanID_ = id; }

	/*!
	  * The load warning from the scan action info. Actually returns the load
	  * warning for the contained scan configuration. If the scan configuration
	  * could not be initialized at all then a generic error message is returned.
	  */
	virtual QString dbLoadWarnings() const;

protected:
	/*!
	  * A reference to the contained scan config, as a base db object. Used in
	  * saving to the db.
	  */
	AMDbObject* dbGetConfig() const;

	/*!
	  * Populates the scan config from the provided base db object. Used in loading
	  * from the db.
	  */
	void dbLoadConfig(AMDbObject *newConfig);

protected slots:
	/*!
	  * Handles a notification that the contained scan config has been altered.
	  * Updates the relevant information in the scan action info to reflect the
	  * config.
	  */
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
