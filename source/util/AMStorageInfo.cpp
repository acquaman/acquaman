#include "AMStorageInfo.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>

#ifndef Q_WS_MAC
#include <mntent.h>
#include <errno.h>
#include <sys/statvfs.h>
#endif
AMStorageInfo::AMStorageInfo()
{
	bytesTotal_ = -1;
	bytesFree_ = -1;
	bytesAvailable_ = -1;
	blockSize_ = -1;
	isReadOnly_ = false;
	isReady_ = false;
	isValid_ = false;	
}

AMStorageInfo::AMStorageInfo(const QString& path)
{
	bytesTotal_ = -1;
	bytesFree_ = -1;
	bytesAvailable_ = -1;
	blockSize_ = -1;
	isReadOnly_ = false;
	isReady_ = false;
	isValid_ = false;

	setPath(path);
}

AMStorageInfo::AMStorageInfo(const QDir& directory)
{
	bytesTotal_ = -1;
	bytesFree_ = -1;
	bytesAvailable_ = -1;
	blockSize_ = -1;
	isReadOnly_ = false;
	isReady_ = false;
	isValid_ = false;

	setPath(directory.absolutePath());
}

AMStorageInfo::AMStorageInfo(const AMStorageInfo& other)
{
	(*this) = other;
}

AMStorageInfo::~AMStorageInfo()
{
	// Class holds no resources.
}

AMStorageInfo &AMStorageInfo::operator =(const AMStorageInfo& other)
{
	bytesTotal_ = other.bytesTotal_;
	bytesFree_ = other.bytesFree_;
	bytesAvailable_ = other.bytesAvailable_;
	blockSize_ = other.blockSize_;
	isReadOnly_ = other.isReadOnly_;
	isReady_ = other.isReady_;
	isValid_ = other.isValid_;

	setPath(other.rootPath_);

	return *this;
}

void AMStorageInfo::setPath(const QString &path)
{
	if(rootPath_ != path) {

		rootPath_ = path;
		refresh();
	}
}

QString AMStorageInfo::rootPath() const
{
	return rootPath_;
}

QByteArray AMStorageInfo::device() const
{
	return device_;
}

QByteArray AMStorageInfo::fileSystemType() const
{
	return fileSystemType_;
}

QString AMStorageInfo::name() const
{
	return name_;
}

QString AMStorageInfo::displayName() const
{
	if(name_.isEmpty()) {

		return rootPath_;
	} else {

		return name_;
	}
}

qint64 AMStorageInfo::bytesTotal() const
{
	return bytesTotal_;
}

qint64 AMStorageInfo::bytesFree() const
{
	return bytesFree_;
}

qint64 AMStorageInfo::bytesAvailable() const
{
	return bytesAvailable_;
}

int AMStorageInfo::blockSize() const
{
	return blockSize_;
}


bool AMStorageInfo::isRoot() const
{
	return *this == AMStorageInfo::root();
}

bool AMStorageInfo::isReadOnly() const
{
	return isReadOnly_;
}

bool AMStorageInfo::isReady() const
{
	return isReady_;
}

bool AMStorageInfo::isValid() const
{
	return isValid_;
}

void AMStorageInfo::refresh()
{
#ifdef Q_WS_MAC
	return;
#else


	// Initialize root path

	rootPath_ = QFileInfo(rootPath_).canonicalFilePath();

	if(!rootPath_.isEmpty()) {

		QByteArray buffer = QByteArray(1024, 0);
		mntent mountEntry;

		FILE* filePointer = ::setmntent("/etc/mtab", "r");

		int maxLength = 0;
		const QString oldRootPath = rootPath_;
		rootPath_.clear();

		while(::getmntent_r(filePointer,
		                    &mountEntry,
		                    buffer.data(),
		                    buffer.size()) != 0) {

			const QString currentMountDirectory = QFile::decodeName(mountEntry.mnt_dir);
			const QByteArray fileSystemType = QByteArray(mountEntry.mnt_fsname);

			if(isPseudoFs(currentMountDirectory, fileSystemType)) {

				continue;
			}

			if(oldRootPath.startsWith(currentMountDirectory) &&
			        maxLength < currentMountDirectory.length()) {

				maxLength = currentMountDirectory.length();
				rootPath_ = currentMountDirectory;
				device_ = QByteArray(mountEntry.mnt_fsname);
				fileSystemType_ = fileSystemType;
			}
		}
	}

	if(rootPath_.isEmpty()) {
		return;
	}

	// Get file system stats

	struct statvfs64 statFileSystemBuffer;
	int result;

	do {
		result = ::statvfs64(QFile::encodeName(rootPath_).constData(),
		                     &statFileSystemBuffer);

	} while (result == -1 && (*__errno_location ()) == 4);

	if(result == 0) {

		isValid_ = true;
		isReady_ = true;

		bytesTotal_ = statFileSystemBuffer.f_blocks * statFileSystemBuffer.f_frsize;
		bytesFree_ = statFileSystemBuffer.f_bfree * statFileSystemBuffer.f_frsize;
		bytesAvailable_ = statFileSystemBuffer.f_bavail * statFileSystemBuffer.f_frsize;
		blockSize_ = statFileSystemBuffer.f_bsize;
		isReadOnly_ = (statFileSystemBuffer.f_flag & ST_RDONLY) != 0;
	}

	// Get the file name

	const char pathDiskByLabel[] = "/dev/disk/by-label";

	QDirIterator directoryIterator(pathDiskByLabel, QDir::NoDotAndDotDot);

	while(directoryIterator.hasNext()) {

		directoryIterator.next();
		QFileInfo fileInfo(directoryIterator.fileInfo());

		if(fileInfo.isSymLink() && fileInfo.symLinkTarget().toLocal8Bit() == device_) {

			name_ = fileInfo.fileName();
		}
	}

#endif
}

QList<AMStorageInfo> AMStorageInfo::mountedVolumes()
{
#ifdef Q_WS_MAC
	return QList<AMStorageInfo>();
#else

	QByteArray buffer = QByteArray(1024, 0);
	mntent mountEntry;

	FILE* filePointer = ::setmntent("/etc/mtab", "r");

	QList<AMStorageInfo> volumes;

	while(::getmntent_r(filePointer,
	                    &mountEntry,
	                    buffer.data(),
	                    buffer.size()) != 0) {

		const QString currentMountDir = QFile::decodeName(mountEntry.mnt_dir);
		const QByteArray fileSystemType = QByteArray(mountEntry.mnt_fsname);

		if(!isPseudoFs(currentMountDir, fileSystemType)) {

			volumes.append(AMStorageInfo(currentMountDir));
		}

	}

	return volumes;
#endif
}

AMStorageInfo AMStorageInfo::root()
{
	return AMStorageInfo("/");
}

bool AMStorageInfo::isPseudoFs(const QString &mountPath, const QByteArray &mountType)
{
	if(mountPath.startsWith("/dev")
	        || mountPath.startsWith("/proc")
	        || mountPath.startsWith("/sys")
	        || mountPath.startsWith("/var/run")
	        || mountPath.startsWith("/var/lock")) {

		return true;
	}

	if(mountType == "tmpfs"
	        || mountType == "rootfs"
	        || mountType == "rpc_pipefs") {

		return true;
	}

	return false;
}

