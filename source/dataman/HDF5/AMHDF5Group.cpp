#include "AMHDF5Group.h"

#include "util/AMErrorMonitor.h"

AMHDF5Group::AMHDF5Group(const QString &name, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	groupID_ = 0;
}

AMHDF5Group::~AMHDF5Group()
{
	if(isOpen())
		closeHDF5Group();
}

bool AMHDF5Group::isOpen() const
{
	return groupID_ > 0;
}

bool AMHDF5Group::createHDF5Group(hid_t fileID)
{
	if (groupID_ != 0){
		AMErrorMon::alert(this, AMHDF5GROUP_INVALID_CREATE_OPTION, QString("Create: Group %1 is already open.").arg(name_));
		return false;
	}

	if(fileID <= 0){
		AMErrorMon::alert(this, AMHDF5GROUP_INVALID_CREATE_OPTION, QString("Create: Cannot create group %1 in a closed file.").arg(name_));
		return false;
	}

	hid_t groupID = H5Gcreate2(fileID, name_.toUtf8().constData(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if(groupID < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_INVALID_CREATE_OPTION, QString("Create: Group %1 could not be created at the driver level").arg(name_));
		//HDF5 Error for group creation failure.
		return false;
	}

	groupID_ = groupID;

	return true;
}

bool AMHDF5Group::openHDF5Group(hid_t fileID)
{
	if(groupID_ != 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_ALREADY_OPEN, QString("Open: Group %1 is already open.").arg(name_));
		return false;
	}

	if(fileID <= 0){
		AMErrorMon::alert(this, AMHDF5GROUP_FILE_IN_BAD_STATE, QString("Open: Group %1 cannot be opened from a closed file!").arg(name_));
		return false;
	}

	hid_t groupID = H5Gopen2(fileID, name_.toUtf8().constData(), H5P_DEFAULT);

	if(groupID < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_OPEN_FAILED, QString("Open: Group %1 could not be opened at the driver level.").arg(name_));
		//HDF5 Error response for failing to open group.
		return false;
	}

	groupID_ = groupID;

	return true;
}

bool AMHDF5Group::closeHDF5Group()
{
	if(groupID_ == 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_ALREADY_CLOSED, QString("Close: Group %1 is not open and therefore cannot be closed!").arg(name_));
		return false;
	}

	herr_t closeStatus = H5Gclose(groupID_);

	if(closeStatus < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_CLOSE_FAILED, QString("Close: Group %1 DID NOT close successfully.").arg(name_));
		//HDF5 error for failing to close group.
		return false;
	}

	groupID_ = 0;
	return true;
}

bool AMHDF5Group::flushHDF5Group()
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_FLUSH_FAILED, QString("Flush: Group %1 cannot be flushed with the file closed.").arg(name_));
		//HDF5 error for flushing a group with a closed file.
		return false;
	}

	herr_t flushStatus = H5Gflush(groupID_);

	if(flushStatus < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_FLUSH_FAILED, QString("Flush: Group %1 flush action failed.").arg(name_));
		//HDF5 error for a flush call fail at the HDF5 level.
		return false;
	}

	return true;
}
