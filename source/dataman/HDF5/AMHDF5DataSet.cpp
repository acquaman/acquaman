#include "AMHDF5DataSet.h"

#include "dataman/HDF5/AMHDF5DataSpace.h"
#include "util/AMErrorMonitor.h"

AMHDF5DataSet::AMHDF5DataSet(hid_t fileId, const QString &name, QObject *parent)
	: QObject(parent)
{
	fileId_ = fileId;
	name_ = name;
	id_ = 0;
}

AMHDF5DataSet::~AMHDF5DataSet()
{
	if (isOpen())
		 close();
}

bool AMHDF5DataSet::isOpen() const
{
	return id_ > 0;
}

hid_t AMHDF5DataSet::id() const
{
	return id_;
}

QString AMHDF5DataSet::name() const
{
	return name_;
}

bool AMHDF5DataSet::readData(double *data) const
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not read from %1 because it is not open.").arg(name_));
		return false;
	}

	herr_t status = H5Dread(id_, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	if (status < 0) {

		// HDF5 error + error mon.
		return false;
	}

	return true;
}

bool AMHDF5DataSet::writeData(double *data)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not write to %1 because it is not open.").arg(name_));
		return false;
	}

	herr_t status = H5Dwrite(id_, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	if (status < 0) {

		// HDF5 error + error mon.
		return false;
	}

	return true;
}

bool AMHDF5DataSet::fill(double value, double *data)
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not write to %1 because it is not open.").arg(name_));
		return false;
	}

	hid_t spaceId = dataSpaceId();
	herr_t status = H5Dfill(&value, H5T_NATIVE_DOUBLE, data, H5T_NATIVE_DOUBLE, spaceId);
	AMHDF5DataSpace::close(spaceId);

	if (status < 0) {

		// HDF5 error + error mon.
		return false;
	}

	return true;
}

bool AMHDF5DataSet::create(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum)
{
	if (isOpen()){

		AMErrorMon::alert(this, 0, QString("Could not create %1 because it is already open.").arg(name_));
		return false;
	}

	if (fileId_ < 1) {

		AMErrorMon::alert(this, 0, "Could not create the file because there was no valid HDF5 file to put it in.");
		return false;
	}

	if (name_.isEmpty()){

		AMErrorMon::alert(this, 0, "Can't create a data set with no name.");
		return false;
	}

	AMHDF5DataSpace *dataSpace = 0;

	if (rank == 0) {

		dataSpace = new AMHDF5DataSpace(AMHDF5DataSpaceInfo::Scaler);

		if (!dataSpace->create()){

			dataSpace->deleteLater();
			AMErrorMon::alert(this, 0, "Scaler data space could not be created.");
			return false;
		}
	}

	else if (rank > 0) {

		dataSpace = new AMHDF5DataSpace(AMHDF5DataSpaceInfo::Simple);

		if (!dataSpace->createSimple(rank, initial, maximum)){

			dataSpace->deleteLater();
			AMErrorMon::alert(this, 0, "Simple data space could not be created.");
			return false;
		}
	}

	else {

		dataSpace = new AMHDF5DataSpace(AMHDF5DataSpaceInfo::Null);

		if (!dataSpace->create()){

			dataSpace->deleteLater();
			AMErrorMon::alert(this, 0, "Null data space could not be created.");
			return false;
		}
	}

	hid_t dataSetId = H5Dcreate2(fileId_, name_.toUtf8().constData(), H5T_NATIVE_DOUBLE, dataSpace->id(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	dataSpace->close();
	dataSpace->deleteLater();

	if (dataSetId < 0) {

		// HDF5 error + error mon.
		return false;
	}

	id_ = dataSetId;

	return true;
}

bool AMHDF5DataSet::open()
{
	if (isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not open data set %1 because it is already open.").arg(name_));
		return false;
	}

	hid_t dataSetId = H5Dopen2(fileId_, name_.toUtf8().constData(), H5P_DEFAULT);

	if (dataSetId < 0) {

		// HDF5 error + error mon.
		return false;
	}

	id_ = dataSetId;

	return true;
}

bool AMHDF5DataSet::close()
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Could not close %1 because either it is already closed or was never opened.").arg(name_));
		return false;
	}

	herr_t status = H5Dclose(id_);

	if (status >= 0){

		id_ = 0;
		return true;
	}

	else {
		// HDF5 error + error mon.
		return false;
	}
}

bool AMHDF5DataSet::flush()
{
	if (!isOpen()) {

		AMErrorMon::alert(this, 0, QString("Can not flush %1 to disk because it is currently not open.").arg(name_));
		return false;
	}

	herr_t status = H5Dflush(id_);

	if (status < 0){

		// HDF5 error + error mon.
		return false;
	}

	return true;
}

hid_t AMHDF5DataSet::dataSpaceId() const
{
	if (!isOpen()){

		AMErrorMon::alert(this, 0, QString("Can not retrieve the data space ID for %1 because it is not open.").arg(name_));
		return false;
	}

	hid_t spaceId = H5Dget_space(id_);

	if (spaceId < 0){

		// HDF5 error + error mon.
		return false;
	}

	return spaceId;
}

