#ifndef VESPERSDATABASEDUPLICATEENTRYOBJECT_H
#define VESPERSDATABASEDUPLICATEENTRYOBJECT_H

#include "dataman/database/AMDatabase.h"

/// This class does all the work to fix all the duplicate entries for a scan.
class VESPERSDatabaseDuplicateEntryObject
{
public:
	/// Constructor.  Takes the id of the scan and the database that it needs to work with.
	VESPERSDatabaseDuplicateEntryObject(AMDatabase *database, int scanId);

protected:
	/// The database the object works with.
	AMDatabase *database_;
	/// The id of the scan to check.
	int scanId_;
};

#endif // VESPERSDATABASEDUPLICATEENTRYOBJECT_H
