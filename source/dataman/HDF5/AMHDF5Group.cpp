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
	if(!isOpen())
		closeHDF5Group();
}

bool AMHDF5Group::isOpen() const
{
	return groupID_ > 0;
}

bool AMHDF5Group::createHDF5Group(hid_t file)
{
	if (groupID_ != 0){
		AMErrorMon::alert(this, 0, QString("Create: Group %1 is already open.").arg(name_));
		return false;
	}

	if(!file > 0){
		AMErrorMon::alert(this, 0, QString("Create: Cannot create group %1 in a closed file.").arg(name_));
	}

	hid_t grpID = 0;

	grpID = H5Gcreate2(file, name_.toUtf8().constData(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if(grpID < 0){
		AMErrorMon::alert(this, 0, QString("Create: Group %1 could not be created at the driver level").arg(name_));
		//HDF5 Error for group creation failure.
		return false;
	}

	groupID_ = grpID;

	return true;
}

bool AMHDF5Group::openHDF5Group(hid_t file)
{
	if(groupID_ != 0){
		AMErrorMon::alert(this, 0, QString("Open: Group %1 is already open.").arg(name_));
	}

	if(!file > 0){
		AMErrorMon::alert(this, 0, QString("Open: Group %1 cannot be opened from a closed file!").arg(name_));
	}

	hid_t grpID = 0;

	grpID = H5Gopen2(file, name_.toUtf8().constData(), H5P_DEFAULT);

	if(grpID < 0){
		AMErrorMon::alert(this, 0, QString("Open: Group %1 could not be opened at the driver level.").arg(name_));
		//HDF5 Error response for failing to open group.
		return false;
	}
	else if (grpID == 0){
		AMErrorMon::alert(this, 0, "Group not found in the specified HDF5 file.");
		return false;
	}

	groupID_ = grpID;

	return true;
}

bool AMHDF5Group::closeHDF5Group()
{
	if(groupID_ == 0){
		AMErrorMon::alert(this, 0, QString("Close: Group %1 is not open and therefore cannot be closed!").arg(name_));
		return false;
	}

	herr_t closeStatus = H5Gclose(groupID_);

	if(closeStatus < 0){
		AMErrorMon::alert(this, 0, QString("Close: Group %1 DID NOT close successfully.").arg(name_));
		//HDF5 error for failing to close group.
		return false;
	}

	groupID_ = 0;
	return true;
}
