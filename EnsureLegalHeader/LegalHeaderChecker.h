#ifndef LEGALHEADERCHECKER_H
#define LEGALHEADERCHECKER_H

#include <QDir>

class LegalHeaderChecker
{
public:
	LegalHeaderChecker(const QString &oldNotice, const QString &newNotice);

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

#endif // LEGALHEADERCHECKER_H
