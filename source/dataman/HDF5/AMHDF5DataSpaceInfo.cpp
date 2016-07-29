#include "AMHDF5DataSpaceInfo.h"

#include "util/AMErrorMonitor.h"

AMHDF5DataSpaceInfo::AMHDF5DataSpaceInfo(hid_t spaceId, QObject *parent)
	: QObject(parent)
{
	id_ = spaceId;
	type_ = Invalid;
	rank_ = -1;
	currentDimensions_ = QVector<hsize_t>();
	maximumDimensions_ = QVector<hsize_t>();

	refresh();
}

AMHDF5DataSpaceInfo::~AMHDF5DataSpaceInfo()
{

}

hid_t AMHDF5DataSpaceInfo::id() const
{
	return id_;
}

AMHDF5DataSpaceInfo::Type AMHDF5DataSpaceInfo::type() const
{
	return type_;
}

int AMHDF5DataSpaceInfo::rank() const
{
	return rank_;
}

QVector<hsize_t> AMHDF5DataSpaceInfo::currentDimensions() const
{
	return currentDimensions_;
}

QVector<hsize_t> AMHDF5DataSpaceInfo::maximumDimensions() const
{
	return maximumDimensions_;
}

bool AMHDF5DataSpaceInfo::refresh()
{
	if (id_ > 0){

		H5S_class_t returnedType = H5Sget_simple_extent_type(id_);

		if (returnedType == H5S_SCALAR)
			type_ = Scaler;

		else if (returnedType == H5S_SIMPLE)
			type_ = Simple;

		else if (returnedType == H5S_NULL)
			type_ = Null;

		else if (returnedType == H5S_NO_CLASS){

			type_ = Invalid;
			AMErrorMon::alert(this, AMHDF5DATASPACEINFO_DOES_NOT_HAVE_VALID_TYPE, "Data space has no valid type.");
			return false;
		}

		rank_ = H5Sget_simple_extent_dims(id_, currentDimensions_.data(), maximumDimensions_.data());
	}

	else {

		type_ = Invalid;
		rank_ = -1;
		currentDimensions_ = QVector<hsize_t>();
		maximumDimensions_ = QVector<hsize_t>();

		AMErrorMon::alert(this, AMHDF5DATASPACEINFO_INVALID_ID, "Data space ID was not valid.");
		return false;
	}

	return true;
}

