#include "AMHDF5Attribute.h"

#include "util/AMErrorMonitor.h"

AMHDF5Attribute::AMHDF5Attribute(const QString &name, QObject *parent) : QObject(parent)
{
	name_ = name;
	id_ = 0;
	datatype_ = H5T_C_S1;
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
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_H5A_FAILURE, QString("Exists: HFD5 API encountered an error."));
		// AMHDF5Error dump
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
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_ALREADY_OPEN, QString("Create: Attribute already opened."));
		return false;
	}

	hid_t status = H5Acreate2(locationId, name_.toLocal8Bit().constData(), datatype_, spaceId, H5P_DEFAULT, H5P_DEFAULT);

	if(status < 0 ){
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_CREATE_FAILED, QString("Create: HDF5 api failed to create the attribute."));
		// AMHDF5Error dump
		return false;
	}

	id_ = status;

	return true;
}

bool AMHDF5Attribute::open()
{
	if(isOpen()){
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_OPEN, QString("Open: Attribute already opened."));
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
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSED, QString("Close: Attribute already closed."));
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
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_REMOVE_FAILED, QString("Remove: Attribute cannot be removed when open."));
		return false;
	}

///	Rules about handling deletion of attributes may need to be handled at the file level.
///	Documentation suggests having no attributes open during a deletion.

//	hid_t status = H5Adelete(id_, name_.toLocal8Bit().constData());

//	if(status < 0){
//		// Error
//		return false;
//	}

	// Handle id_ here.
	return true;
}

QString AMHDF5Attribute::read() const
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSED, QString("Read: Attribute not open and cannot be read."));
		return false;
	}

	char* buffer = new char[512]();

	herr_t status = H5Aread(id_, datatype_, buffer);

	if(status < 0){
		// Error
		return "";
	}

	// Move data to object to avoid memory leaks.
	QString returnValue(QString::fromLocal8Bit(buffer));
	delete(buffer); // Cleanup buffer.

	return returnValue;
}

bool AMHDF5Attribute::write(const QString &attributeData)
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSED, QString("Write: Attribute not open and cannot be written."));
		return false;
	}

	if(attributeData.isEmpty()){
		// Error
		return false;
	}

	herr_t status = H5Awrite(id_, datatype_, attributeData.toLocal8Bit().constData());

	if(status < 0){
		// Error
		return false;
	}

	return true;
}

bool AMHDF5Attribute::rename(const QString &newName)
{
	if(!isOpen()){
		AMErrorMon::alert(this, AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSED, QString("Rename: Attribute closed and cannot be renamed."));
		return false;
	}

	herr_t status = H5Arename(id_, name_.toLocal8Bit().constData(), newName.toLocal8Bit().constData());

	if(status < 0){
		// Error
		return false;
	}

	name_ = newName;

	return true;
}
