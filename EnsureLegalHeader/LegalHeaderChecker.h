#ifndef LEGALHEADERCHECKER_H
#define LEGALHEADERCHECKER_H

#include <QDir>

class LegalHeaderChecker
{
public:
	LegalHeaderChecker();

protected:
	void recurseDirectories(const QString &currentPath, const QStringList &directories);
};

#endif // LEGALHEADERCHECKER_H
