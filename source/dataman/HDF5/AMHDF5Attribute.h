#ifndef AMHDF5ATTRIBUTE_H
#define AMHDF5ATTRIBUTE_H

#include <QObject>

#include <hdf5.h>

/// This class is to handle interfacing with H5A allowing for creating and handling HDF5 attributes.
/// It takes a reference to a HDF5 object (group, dataset, or committed datatype) and allows  for attaching an attribute.
/// An attribute is meta data inserted into the header of an object and is accessible without looking at the contents of
/// an object's data itself.
class AMHDF5Attribute : public QObject
{
	Q_OBJECT
public:

	// Constructor for an attribute object.
	explicit AMHDF5Attribute(const QString &name, hid_t id_, QObject *parent = 0);
	// Destructor.
	virtual ~AMHDF5Attribute();
	// Checks to see if an attribute exists for the specified object.
	bool exists();

signals:

public slots:
	// Creates an attribute for the associated object.
	bool create();
	// Opens an attribute for the assosciated object.
	bool open();
	// Closes an open attribute for the associated object.
	bool close();
	// Removes an existing attributes from an object.
	bool remove();
	// Flushes new attribute changes to disk.
	bool flush();

private:
	// Holds the object id for this object.
	hid_t id_;
	// Holds the name for the attribute.
	QString name_;

};

#endif // AMHDF5ATTRIBUTE_H
