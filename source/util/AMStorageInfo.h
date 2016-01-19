#ifndef AMSTORAGEINFO_H
#define AMSTORAGEINFO_H

#include <QDir>
#include <QByteArray>

class AMStorageInfo
{
public:
    AMStorageInfo();
	explicit AMStorageInfo(const QString& path);
	explicit AMStorageInfo(const QDir& directory);
	AMStorageInfo(const AMStorageInfo& other);

	~AMStorageInfo();

	AMStorageInfo &operator=(const AMStorageInfo& other);

	void setPath(const QString& path);

	QString rootPath() const;
	QByteArray device() const;
	QByteArray fileSystemType() const;
	QString name() const;
	QString displayName() const;

	qint64 bytesTotal() const;
	qint64 bytesFree() const;
	qint64 bytesAvailable() const;
	int blockSize() const;

	inline bool isRoot() const;

	bool isReadOnly() const;
	bool isReady() const;
	bool isValid() const;

	void refresh();

	static QList<AMStorageInfo> mountedVolumes();
	static AMStorageInfo root();

protected:
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
