#ifndef HEADERNOTICECHECKER_H
#define HEADERNOTICECHECKER_H

#include <QDir>

class HeaderNoticeChecker
{
public:
	HeaderNoticeChecker(const QString &oldNotice, const QString &newNotice, const QString &directoryPath);

protected:
	void recurseDirectories(const QString &currentPath, const QStringList &directories);

protected:
	QString oldNotice_;
	QString newNotice_;
	QString anyNotice_;
	QString fullNotice_;
	QStringList oldToNew_;
	QStringList nothingToNew_;
	QStringList ignoreDirectories_;
};

#endif // HEADERNOTICECHECKER_H
