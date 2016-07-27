#ifndef AMHDF5GROUP_H
#define AMHDF5GROUP_H

#include <QObject>

#include <hdf5.h>

#include <QString>

#include "AMHDF5File.h"


/// This class allows for adding groups to AMHDF5 files. Groups are the highest level of data
/// organiziation in an HDF5 file and every HDF5 file must have at least one group.
class AMHDF5Group : public QObject
{
	Q_OBJECT

public:

	/// Constructor. Takes the name of the HDF5 Group. Does not create the group itself.
	explicit AMHDF5Group(const QString &name, QObject *parent = 0);
	/// Deconstructor for the group object.
	virtual ~AMHDF5Group();
	/// Checks if a group is currently open.
	bool isOpen() const;

signals:

public slots:
	/// Create the HDF5 group for a specified file and associates that group handle with this object.
	bool createHDF5Group(hid_t file);
	/// Opens an existing HDF5 group for access and associates the handle with this object.
	bool openHDF5Group(hid_t file);
	/// Closes a currently open HDF5 group and resets the group handle ID.
	bool closeHDF5Group();

protected:
	/// The name of the group to be made.
	QString name_;
	/// ID for the currently open HDF5 group. Zero if no group is open.
	int groupID_;
};


#endif // AMHDF5GROUP_H
