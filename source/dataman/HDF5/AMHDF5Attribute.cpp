#include "AMHDF5Attribute.h"

AMHDF5Attribute::AMHDF5Attribute(const QString &name, QObject *parent) : QObject(parent)
{
	name_ = name;
	id_ = 0;
}

AMHDF5Attribute::~AMHDF5Attribute()
{

}

bool AMHDF5Attribute::exists() const
{
	htri_t status = H5Aexists(id_, name_.toLocal8Bit().constData());

	// Two cases:
	// Zero implies not found, but successful.
	// Negative implies an error occurred.

	if(status < 0){
		//Error
		return false;
	}

	if(status == 0){
		return false;
	}

	return true;
}

bool AMHDF5Attribute::isOpen() const
{
	return id_ > 0;
}

bool AMHDF5Attribute::create(hid_t locationId, hid_t spaceId)
{
	if(isOpen()){
		// Error
		return false;
	}

	hid_t status = H5Acreate2(locationId, name_.toLocal8Bit().constData(), H5T_C_S1, spaceId, H5P_DEFAULT, H5P_DEFAULT);

	if(status < 0 ){
		// Error
		return false;
	}

	id_ = status;

	return true;
}

bool AMHDF5Attribute::open()
{
	if(isOpen()){
		// Error
		return false;
	}

	hid_t status = H5Aopen(id_, name_.toLocal8Bit().constData(), H5P_DEFAULT);

	if (status < 0 ){
		// Error
		return false;
	}

	id_ = status;

	return true;
}

bool AMHDF5Attribute::close()
{
	if(!isOpen()){
		// Error
		return false;
	}

	hid_t status = H5Aclose(id_);

	if(status < 0){
		// Error
		return false;
	}

	id_ = 0;

	return true;
}

bool AMHDF5Attribute::remove()
{
	if(!isOpen()){
		// Error
		return false;
	}
	// Rules about handling deletion of attributes may need to be handled at the file level.
	// Documentation suggests having no attributes open during a deletion.
//	hid_t status = H5Adelete(id_, name_.toLocal8Bit().constData());

//	if(status < 0){
//		// Error
//		return false;
//	}

	// Handle id_ here.
	return true;
}
