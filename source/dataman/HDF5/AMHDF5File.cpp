#include "AMHDF5File.h"

#include "util/AMErrorMonitor.h"

AMHDF5File::AMHDF5File(const QString &name, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	id_ = 0;
}

AMHDF5File::~AMHDF5File()
{
	if (!isOpen())
		closeHDF5File();
}

bool AMHDF5File::isHDF5File() const
{
	htri_t returnValue = H5Fis_hdf5(name_.toUtf8().constData());

	if (returnValue > 0)
		return true;

	else if (returnValue == 0)
		return false;

	else{
		// Error stuff.
		return false;
	}
}

bool AMHDF5File::isOpen() const
{
	return id_ > 0;
}

bool AMHDF5File::createHDF5File(AMHDF5File::CreateOption option)
{
	if (id_ != 0){

		AMErrorMon::alert(this, AMHDF5FILE_FILE_ALREADY_OPEN, QString("%1 is already open.").arg(name_));
		return false;
	}

	hid_t fileId = 0;

	if (option == OverwriteExisting)
		fileId = H5Fcreate(name_.toUtf8().constData(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	else if (option == DoNotOverwrite)
		fileId = H5Fcreate(name_.toUtf8().constData(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);

	if (fileId < 0){

		// HDF5 file error + error mon.
		return false;
	}

	else if (fileId == 0){

		AMErrorMon::alert(this, AMHDF5FILE_INVALID_FILE_OPTION, "The provided options to the HDF5 file where not correctly specified.");
		return false;
	}

	// We have a valid file handle now.
	id_ = fileId;

	return true;
}

bool AMHDF5File::closeHDF5File()
{
	if (id_ == 0){

		AMErrorMon::alert(this, AMHDF5FILE_FILE_ALREADY_CLOSED, QString("%1 has already been closed or was never opened.").arg(name_));
		return false;
	}

	herr_t status = H5Fclose(id_);

	if (status > 0){

		id_ = 0;
		return true;
	}

	else {
		// HDF5 error + error mon.
		return false;
	}
}

