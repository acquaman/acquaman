#include "VESPERSDatabaseDuplicateEntryObject.h"

VESPERSDatabaseDuplicateEntryObject::VESPERSDatabaseDuplicateEntryObject(AMDatabase *database, int scanId)
{
	database_ = database;
	scanId_ = scanId;
}
