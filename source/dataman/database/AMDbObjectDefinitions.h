#ifndef AMDBOBJECTDEFINITIONS_H
#define AMDBOBJECTDEFINITIONS_H

namespace AMDbObjectDefinitions {
	/// This enum lists the possible load statuses and is returned by AMDbObject::loadFromDb() as well as by functions in AMDbObjectSupport
	enum AMDbObjectLoadStatus{
		AlreadyLoadedStatus = 0,	///< The AMDbObject is already loaded into the retention system
		NotYetLoadedStatus = 1,		///< The AMDbObject is not yet loaded into the retention system
		SuccessfullyLoadedStatus = 2	///< Whatever operation just occured successfully loaded the AMDbObject into the retention system
	};
}

#endif // AMDBOBJECTDEFINITIONS_H
