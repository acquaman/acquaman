#ifndef AMSTORAGEINFO_H
#define AMSTORAGEINFO_H

#include <QDir>
#include <QByteArray>

/*!
  * A class which provides information about the storage volume at the provided
  * path. The class is based heavily on AMStorageInfo from Qt5.4 (see http://doc.qt.io/qt-5/AMStorageInfo.html)
  * , but without the d-pointer pattern used in Qt implementations.
  * Owing to the low level nature of the class it contains a lot of linux specific
  * implementation details, and will not work (but will compile) on non linux machines.
  * As such a lot of conditional compilation is used to return stub values on Macs
  */
class AMStorageInfo
{
public:

	/*!
	  * Constructs an empty AMStorageInfo object.

	  * Objects created with the default constructor will be invalid and therefore
	  * not ready for use.
	  */
    AMStorageInfo();

	/*!
	  * Constructs a new AMStorageInfo object that gives information about the volume
	  * mounted at path.
	  *
	  * If you pass a directory or file, the AMStorageInfo object will refer to the
	  * volume where this directory or file is located. You can check if the created
	  * object is correct using the isValid() method.
	  */
	explicit AMStorageInfo(const QString& path);

	/*!
	  * Constructs a new AMStorageInfo object that gives information about the volume
	  * containing the dir folder.
	  */
	explicit AMStorageInfo(const QDir& directory);

	/*!
	  * Constructs a new AMStorageInfo object that is a copy of the other AMStorageInfo
	  * object.
	  */
	AMStorageInfo(const AMStorageInfo& other);

	/*!
	  * Destroys the AMStorageInfo object and frees its resources.
	  */
	~AMStorageInfo();

	AMStorageInfo &operator=(const AMStorageInfo& other);

	/*!
	  * Sets this AMStorageInfo object to the filesystem mounted where path is located.
	  * path can either be a root path of the filesystem, a directory, or a file
	  * within that filesystem.
	  */
	void setPath(const QString& path);

	/*!
	  * Returns the mount point of the filesystem this AMStorageInfo object represents.
	  * NOTE: The value returned by rootPath() is the real mount point of a volume,
	  * and may not be equal to the value passed to the constructor or setPath()
	  * method. For example, if you have only the root volume in the system, and
	  * pass '/directory' to setPath(), then this method will return '/'.
	  */
	QString rootPath() const;

	/*!
	  * Returns the device for this volume.
	  * For example this returns the devpath like /dev/sda0 for local storages.
	  */
	QByteArray device() const;

	/*!
	  * Returns the type name of the filesystem.
	  */
	QByteArray fileSystemType() const;

	/*!
	  * Returns the human-readable name of a filesystem, usually called label.
	  * NOTE: Requires udev to be present in the system.
	  */
	QString name() const;

	/*!
	  * Returns the volume's name, if available, or the root path if not.
	  */
	QString displayName() const;

	/*!
	  * Returns the total volume size in bytes.
	  * Returns -1 if AMStorageInfo object is not valid.
	  */
	qint64 bytesTotal() const;

	/*!
	  * Returns the number of free bytes in a volume. Note that if there are quotas
	  * on the filesystem, this value can be larger than the value returned by
	  * bytesAvailable().
	  * Returns -1 if AMStorageInfo object is not valid.
	  */
	qint64 bytesFree() const;

	/*!
	  * Returns the size (in bytes) available for the current user. It returns
	  * the total size available if the user is the root user or a system
	  * administrator.
	  * This size can be less than or equal to the free size returned by bytesFree()
	  * function.
	  */
	qint64 bytesAvailable() const;

	/*!
	  * The size of blocks of the current filesystem.
	  */
	int blockSize() const;

	/*!
	  * Returns true if this AMStorageInfo represents the system root volume;
	  * false otherwise.
	  */
	bool isRoot() const;

	/*!
	  * Returns true if the current filesystem is protected from writing; false
	  * otherwise.
	  */
	bool isReadOnly() const;

	/*!
	  * Returns true if the current filesystem is ready to work; false otherwise.
	  * For example, false is returned if the CD volume is not inserted.
	  */
	bool isReady() const;

	/*!
	  * Returns true if the AMStorageInfo specified by rootPath exists and is
	  * mounted correctly.
	  */
	bool isValid() const;

	/*!
	  * Resets AMStorageInfo's internal cache.
	  * AMStorageInfo caches information about storage to speed up performance.
	  * AMStorageInfo retrieves information during object construction and/or when
	  * calling the setPath() method. You have to manually reset the cache by calling
	  * this function to update storage information.
	  */
	void refresh();

	/*!
	  * Returns the list of AMStorageInfo objects that corresponds to the list of
	  * currently mounted filesystems.
	  */
	static QList<AMStorageInfo> mountedVolumes();

	/*!
	  * Returns a AMStorageInfo object that represents the system root volume.
	  */
	static AMStorageInfo root();

protected:

	// Returns true if the provided mount path and type represent a pseudo file
	// system.
	static bool isPseudoFs(const QString& mountPath, const QByteArray& mountType);

	QString rootPath_;
	QByteArray device_;
	QByteArray fileSystemType_;
	QString name_;

	qint64 bytesTotal_;
	qint64 bytesFree_;
	qint64 bytesAvailable_;
	int blockSize_;

	bool isReadOnly_;
	bool isReady_;
	bool isValid_;


};

inline bool operator==(const AMStorageInfo& first, const AMStorageInfo& second)
{
	return first.device() == second.device();
}

#endif // AMSTORAGEINFO_H
