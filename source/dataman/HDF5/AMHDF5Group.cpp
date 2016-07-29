#include "AMHDF5Group.h"

#include "util/AMErrorMonitor.h"

AMHDF5Group::AMHDF5Group(hid_t fileId, const QString &name, QObject *parent)
	: QObject(parent)
{
	fileId_ = fileId;
	name_ = name;
	id_ = 0;
}

AMHDF5Group::~AMHDF5Group()
{
	if(isOpen())
		close();
}

bool AMHDF5Group::isOpen() const
{
	return id_ > 0;
}

hid_t AMHDF5Group::id() const
{
	return id_;
}

bool AMHDF5Group::create()
{
	if (isOpen()){
		AMErrorMon::alert(this, AMHDF5GROUP_INVALID_CREATE_OPTION, QString("Create: Group %1 is already open.").arg(name_));
		return false;
	}

	hid_t groupID = H5Gcreate2(fileId_, name_.toUtf8().constData(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if(groupID < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_INVALID_CREATE_OPTION, QString("Create: Group %1 could not be created at the driver level").arg(name_));
		//HDF5 Error for group creation failure.
		return false;
	}

	id_ = groupID;

	return true;
}

bool AMHDF5Group::open()
{
	if(isOpen()){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_ALREADY_OPEN, QString("Open: Group %1 is already open.").arg(name_));
		return false;
	}

	hid_t groupID = H5Gopen2(fileId_, name_.toUtf8().constData(), H5P_DEFAULT);

	if(groupID < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_OPEN_FAILED, QString("Open: Group %1 could not be opened at the driver level.").arg(name_));
		//HDF5 Error response for failing to open group.
		return false;
	}

	id_ = groupID;

	return true;
}

bool AMHDF5Group::close()
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_ALREADY_CLOSED, QString("Close: Group %1 is not open and therefore cannot be closed!").arg(name_));
		return false;
	}

	herr_t closeStatus = H5Gclose(id_);

	if(closeStatus < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_CLOSE_FAILED, QString("Close: Group %1 DID NOT close successfully.").arg(name_));
		//HDF5 error for failing to close group.
		return false;
	}

	id_ = 0;
	return true;
}

bool AMHDF5Group::flush()
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_FLUSH_FAILED, QString("Flush: Group %1 cannot be flushed with the file closed.").arg(name_));
		//HDF5 error for flushing a group with a closed file.
		return false;
	}

	herr_t flushStatus = H5Gflush(id_);

	if(flushStatus < 0){
		AMErrorMon::alert(this, AMHDF5GROUP_GROUP_FLUSH_FAILED, QString("Flush: Group %1 flush action failed.").arg(name_));
		//HDF5 error for a flush call fail at the HDF5 level.
		return false;
	}

	return true;
}
