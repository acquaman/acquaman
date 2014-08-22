#include "AMStorageRing.h"

#include "util/AMErrorMonitor.h"

AMStorageRing *AMStorageRing::instance_ = 0;

AMStorageRing::AMStorageRing(QObject *parent)
	: QObject(parent)
{
	ringCurrent_ = 0;
	beamAvailable_ = false;
}

AMStorageRing::~AMStorageRing()
{

}

AMStorageRing *AMStorageRing::storageRing()
{
	if(!instance_)
		AMErrorMon::alert(0, AMSTORAGERING_NOT_CREATED_YET, "No storage ring created yet.  You need to call YourStorageRing::storageRing() before calling AMStorageRing::storageRing().");

   return instance_;
}

void AMStorageRing::releaseStorageRing()
{
	if (instance_){

		delete instance_;
		instance_ = 0;
	}
}

double AMStorageRing::ringCurrent() const
{
	if (ringCurrent_)
		return ringCurrent_->value();

	return -1.0;
}

AMControl *AMStorageRing::ringCurrentControl() const
{
	return ringCurrent_;
}

bool AMStorageRing::beamAvailable() const
{
	return beamAvailable_;
}
