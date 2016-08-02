#ifndef AMHDF5GROUP_H
#define AMHDF5GROUP_H

#include <QObject>

#include <hdf5.h>

#include <QString>

#include "AMHDF5Error.h"

#define AMHDF5GROUP_INVALID_CREATE_OPTION 667000
#define AMHDF5GROUP_GROUP_ALREADY_OPEN 667001
#define AMHDF5GROUP_FILE_IN_BAD_STATE 667002
#define AMHDF5GROUP_GROUP_OPEN_FAILED 667003
#define AMHDF5GROUP_GROUP_ALREADY_CLOSED 667004
#define AMHDF5GROUP_GROUP_CLOSE_FAILED 667005
#define AMHDF5GROUP_GROUP_FLUSH_FAILED 667006

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
	/// Returns the group ID currently held by AMHDF5Group.
	hid_t id() const { return groupID_;}

signals:

public slots:
	/// Create the HDF5 group for a specified file and associates that group handle with this object.
	bool create(hid_t fileID);
	/// Opens an existing HDF5 group for access and associates the handle with this object.
	bool open(hid_t fileID);
	/// Closes a currently open HDF5 group and resets the group handle ID.
	bool close();
	/// Flushes the group and associated file to disk.
	bool flush();

protected:
	/// The name of the group to be made.
	QString name_;
	/// ID for the currently open HDF5 group. Zero if no group is open.
	int groupID_;
	///
	AMHDF5Error* error_;
};


#endif // AMHDF5GROUP_H
