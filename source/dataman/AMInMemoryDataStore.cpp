#include "AMInMemoryDataStore.h"

AMInMemoryDataStore::AMInMemoryDataStore()
	: AMDataStore()
{
	// with no scan axes yet:
	dataRoot_ = 0;
}

AMInMemoryDataStore::~AMInMemoryDataStore() {
	if(dataRoot_) {
		clearScanDataPointsImplementationRecursive(dataRoot_, scanAxesCount()-1, false);
		delete dataRoot_;
	}
}
