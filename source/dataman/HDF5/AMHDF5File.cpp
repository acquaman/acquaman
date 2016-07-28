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
	if (isOpen())
		close();
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

int AMHDF5File::fileSize() const
{
	if (!isOpen())
		return -1;

	hsize_t size = 0;
	herr_t status = H5Fget_filesize(id_, &size);

	if (status < 0){

		// HDF5 File error + error mon.
		return -1;
	}

	int sizeInMegaBytes = int(size*1e-6);

	return sizeInMegaBytes;
}

bool AMHDF5File::create(AMHDF5File::CreateOption option)
{
	if (isOpen()){

		AMErrorMon::alert(this, AMHDF5FILE_FILE_ALREADY_OPEN, QString("Create: %1 is already open.").arg(name_));
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

		AMErrorMon::alert(this, AMHDF5FILE_INVALID_CREATE_OPTION, "The provided options to the HDF5 file where not correctly specified.");
		return false;
	}

	// We have a valid file handle now.
	id_ = fileId;

	return true;
}

bool AMHDF5File::open(AMHDF5File::OpenOption option)
{
	if (isOpen()){

		AMErrorMon::alert(this, AMHDF5FILE_FILE_ALREADY_OPEN, QString("Open: %1 is already open.").arg(name_));
		return false;
	}

	if (!isHDF5File()){

		AMErrorMon::alert(this, AMHDF5FILE_NOT_HDF5_FILE, QString("%1 is not a valid file name.").arg(name_));
		return false;
	}

	hid_t fileId = 0;

	if (option == ReadWrite)
		fileId = H5Fopen(name_.toUtf8().constData(), H5F_ACC_RDWR, H5P_DEFAULT);

	else if (option == ReadOnly)
		fileId = H5Fopen(name_.toUtf8().constData(), H5F_ACC_RDONLY, H5P_DEFAULT);

	if (fileId < 0){

		// HDF5 file error + error mon.
		return false;
	}

	else if (fileId == 0){

		AMErrorMon::alert(this, AMHDF5FILE_INVALID_OPEN_OPTION, "The provided options to the HDF5 file where not correctly specified.");
		return false;
	}

	id_ = fileId;
	return true;
}

bool AMHDF5File::close()
{
	if (!isOpen()){

		AMErrorMon::alert(this, AMHDF5FILE_FILE_ALREADY_CLOSED, QString("%1 has already been closed or was never opened.").arg(name_));
		return false;
	}

	herr_t status = H5Fclose(id_);

	if (status >= 0){

		id_ = 0;
		return true;
	}

	else {
		// HDF5 error + error mon.
		return false;
	}
}

bool AMHDF5File::flush()
{
	if (!isOpen()){

		AMErrorMon::alert(this, AMHDF5FILE_FLUSH_FILE_NOT_OPEN, QString("%1 can't be flushed because it is not open.").arg(name_));
		return false;
	}

	herr_t status = H5Fflush(id_, H5F_SCOPE_GLOBAL);

	if (status < 0){

		// HDF5 error + error mon.
		return false;
	}

	return true;
}

bool AMHDF5File::addGroup(const QString &groupName)
{
	AMHDF5Group *group = new AMHDF5Group(groupName);

	if(isOpen() && group->create(id_)){

		groups_.insert(groupName, group);
		return true;
	}

	group->deleteLater();

	return false;
}

bool AMHDF5File::openGroup(const QString &groupName)
{
	if(isOpen() && !groups_.contains(groupName)){

		AMHDF5Group *groupToOpen = new AMHDF5Group(groupName);

		if(!groupToOpen->open(id_)){

			groupToOpen->deleteLater();
			return false;
		}

		groups_.insert(groupName, groupToOpen);

		return true;
	}

	return false;
}

AMHDF5Group* AMHDF5File::groupByName(const QString &groupName) const
{
	return groups_.value(groupName, 0);
}

AMHDF5DataSet *AMHDF5File::dataSetByName(const QString &dataSetName) const
{
	return dataSets_.value(dataSetName, 0);
}

bool AMHDF5File::closeGroup(const QString &groupName)
{
	if(isOpen() && groups_.contains(groupName)){

		AMHDF5Group* groupToClose = groups_.take(groupName);

		if(groupToClose->close()){

			groupToClose->deleteLater();
			return true;
		}
	}

	return false;
}

bool AMHDF5File::flushGroup(const QString &groupName)
{
	if(isOpen() && groups_.contains(groupName)){

		AMHDF5Group* groupToFlush = groups_.value(groupName);

		if(groupToFlush->flush()){
			return true;
		}
	}

	return false;
}

bool AMHDF5File::addDataSet(const QString &dataSetName, int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not add data set %1 to %2 because the file is not open.").arg(dataSetName).arg(name_));
		return false;
	}

	AMHDF5DataSet *dataSet = new AMHDF5DataSet(id_, dataSetName);

	if(!dataSet->create(rank, initial, maximum)){

		dataSet->deleteLater();
		AMErrorMon::alert(this, 0, QString("Could not create %1.").arg(dataSetName));
		return false;
	}

	dataSets_.insert(dataSetName, dataSet);

	return true;
}

bool AMHDF5File::openDataSet(const QString &dataSetName)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not add data set %1 to %2 because the file is not open.").arg(dataSetName).arg(name_));
		return false;
	}

	AMHDF5DataSet *dataSet = new AMHDF5DataSet(id_, dataSetName);

	if(!dataSet->open()){

		dataSet->deleteLater();
		AMErrorMon::alert(this, 0, QString("Could not open %1.").arg(dataSetName));
		return false;
	}

	dataSets_.insert(dataSetName, dataSet);

	return true;
}

bool AMHDF5File::closeDataSet(const QString &dataSetName)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not close data set %1 because the the file is already closed.").arg(dataSetName));
		return false;
	}

	if (dataSets_.contains(dataSetName)){

		AMHDF5DataSet *dataSet = dataSets_.take(dataSetName);

		if (!dataSet->close()){

			AMErrorMon::alert(this, 0, QString("Could not close %1.").arg(dataSetName));
			dataSet->deleteLater();
			return false;
		}

		dataSet->deleteLater();
	}

	return true;
}

bool AMHDF5File::flushDataSet(const QString &dataSetName)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Could not flush data set %1 because the file was not open.").arg(dataSetName));
		return false;
	}

	if (dataSets_.contains(dataSetName)){

		AMHDF5DataSet *dataSet = dataSets_.value(dataSetName);

		if (!dataSet->flush()){

			AMErrorMon::alert(this, 0, QString("Could not flush %1.").arg(dataSetName));
			return false;
		}
	}

	return true;
}

