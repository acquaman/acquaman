#include "AMHDF5DataSpace.h"

#include "util/AMErrorMonitor.h"

AMHDF5DataSpace::AMHDF5DataSpace(AMHDF5DataSpaceInfo::Type type, QObject *parent)
	: QObject(parent)
{
	type_ = type;
	id_ = 0;
	error_ = new AMHDF5Error;
}

AMHDF5DataSpace::~AMHDF5DataSpace()
{
	if (isOpen())
		close();
}

hid_t AMHDF5DataSpace::id() const
{
	return id_;
}

bool AMHDF5DataSpace::isOpen() const
{
	return id_ > 0;
}

bool AMHDF5DataSpace::create()
{
	if (isOpen()){

		AMErrorMon::alert(this, AMHDF5DATASPACE_SPACE_ALREADY_OPEN, "Could not create a data space because it is already open.");
		return false;
	}

	hid_t spaceId = 0;

	if (type_ == AMHDF5DataSpaceInfo::Scaler)
		spaceId = H5Screate(H5S_SCALAR);

	else if (type_ == AMHDF5DataSpaceInfo::Simple)
		spaceId = H5Screate(H5S_SIMPLE);

	else if (type_ == AMHDF5DataSpaceInfo::Null)
		spaceId = H5Screate(H5S_NULL);

	if (spaceId < 0){

		// HDF5 space error + error mon.
		error_->dumpErrorStack();
		return false;
	}

	else if (spaceId == 0){

		AMErrorMon::alert(this, AMHDF5DATASPACE_INVALID_SPACE_TYPE, "The provided data space type was invalid.");
		return false;
	}

	// We have a valid data space handle now.
	id_ = spaceId;

	return true;
}

bool AMHDF5DataSpace::createSimple(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum)
{
	if (type_ != AMHDF5DataSpaceInfo::Simple)
		type_ = AMHDF5DataSpaceInfo::Simple;

	if (create()){

		if (!setDimensionality(rank, initial, maximum)){

			close();
			return false;
		}
	}

	else {
		return false;
	}

	return true;
}

bool AMHDF5DataSpace::close()
{
	if (!isOpen()){

		AMErrorMon::alert(this, AMHDF5DATASPACE_SPACE_ALREADY_CLOSED, "This data space has already been closed or was never opened.");
		return false;
	}

	herr_t status = H5Sclose(id_);

	if (status >= 0){

		id_ = 0;
		return true;
	}

	else {

		// HDF5 error + error mon.
		error_->dumpErrorStack();
		return false;
	}
}

bool AMHDF5DataSpace::setDimensionality(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum)
{
	if (!isOpen()){

		AMErrorMon::alert(this, AMHDF5DATASPACE_SPACE_ALREADY_OPEN, "Could not set the dimensions to the data space because it was not open.");
		return false;
	}

	if (type_ != AMHDF5DataSpaceInfo::Simple){

		AMErrorMon::alert(this, AMHDF5DATASPACE_INVALID_SPACE_TYPE, "Only Simple data spaces can have dimensions set to them.");
		return false;
	}

	if (rank < 1){

		AMErrorMon::alert(this, AMHDF5DATASPACE_INCORRECT_DIMENSIONS, "Can not have a Simple data space with dimensionality less than 1.");
		return false;
	}

	if (rank == initial.size() && maximum.isEmpty()){

		QVector<hsize_t> unlimitedMaximum = QVector<hsize_t>(rank, H5S_UNLIMITED);
		herr_t status = H5Sset_extent_simple(id_, rank, initial.constData(), unlimitedMaximum.constData());

		if (status < 0){

			// HDF5 error + error mon.
			error_->dumpErrorStack();
			return false;
		}
	}

	else if (rank == initial.size() && initial.size() == maximum.size()){

		herr_t status = H5Sset_extent_simple(id_, rank, initial.constData(), maximum.constData());

		if (status < 0){

			// HDF5 error + error mon.
			error_->dumpErrorStack();
			return false;
		}
	}

	else {

		AMErrorMon::alert(this, AMHDF5DATASPACE_DIMENSIONS_DONT_MATCH, "The size of the dimensionality arrays must match the rank.");
		return false;
	}

	// We successfully set the extent of the data space.
	return true;
}
