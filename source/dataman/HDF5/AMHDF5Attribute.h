#ifndef AMHDF5ATTRIBUTE_H
#define AMHDF5ATTRIBUTE_H

#include <QObject>

#include <hdf5.h>

#define AMHDF5ATTRIBUTE_H5A_FAILURE 687000
#define AMHDF5ATTRIBUTE_ATTRIBUTE_OPEN 687001
#define AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSED 687002
#define AMHDF5ATTRIBUTE_ATTRIBUTE_CREATE_FAILED 687003
#define AMHDF5ATTRIBUTE_ATTRIBUTE_OPEN_FAILED 687004
#define AMHDF5ATTRIBUTE_ATTRIBUTE_CLOSE_FAILED 687005
#define AMHDF5ATTRIBUTE_ATTRIBUTE_REMOVE_FAILED 687006
#define AMHDF5ATTRIBUTE_ATTRIBUTE_READ_FAILED 687007
#define AMHDF5ATTRIBUTE_ATTRIBUTE_READ_FAILED 687008
#define AMHDF5ATTRIBUTE_ATTRIBUTE_RENAME_NAME_INVALID 687009
#define AMHDF5ATTRIBUTE_ATTRIBUTE_RENAME_FAILED 687010

/// This class is to handle interfacing with H5A allowing for creating and handling HDF5 attributes.
/// It takes a reference to a HDF5 object (group, dataset, or committed datatype) and allows  for attaching an attribute.
/// An attribute is meta data inserted into the header of an object and is accessible without looking at the contents of
/// an object's data itself.
class AMHDF5Attribute : public QObject
{
	Q_OBJECT
public:

	/// Constructor for an attribute object.
	explicit AMHDF5Attribute(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5Attribute();
	/// Checks to see if an attribute exists for the specified object.
	bool exists() const;
	/// Checks if this object instance is associated with an open attribute.
	bool isOpen() const;

signals:

public slots:
	/// Creates an attribute for the associated object.
	bool create(hid_t locationId, hid_t spaceId);
	/// Opens an attribute for the assosciated object.
	bool open();
	/// Closes an open attribute for the associated object.
	bool close();
	/// Removes an existing attributes from an object.
	bool remove();
	/// Reads the attribute into a specified buffer.
	QString read() const;
	/// Writes the attribute information.
	bool write(const QString &attributeData);
	/// Rename this attribute.
	bool rename(const QString &newName);

private:
	/// Holds the object id for this object.
	hid_t id_;
	/// Keeps reference to the type of data the attribute holds.
	hid_t datatype_;
	/// Holds the name for the attribute.
	QString name_;

};

#endif // AMHDF5ATTRIBUTE_H
