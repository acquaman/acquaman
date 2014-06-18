#ifndef AMRECURSIVEDIRECTORYCOMPARE_H
#define AMRECURSIVEDIRECTORYCOMPARE_H

#include <QStringList>
class AMRecursiveDirectoryCompare
{
public:
	enum DirectoryCompareResult{
		FullyMatchingResult = 0,
		Side1ModifiedResult = 1,
		Side2ModifiedResult = 2,
		BothSidesModifiedResult = 3,
		InvalidResult
	};

	AMRecursiveDirectoryCompare(const QString &directoryPath1, const QString &directoryPath2);

	AMRecursiveDirectoryCompare::DirectoryCompareResult compare();

protected:
	bool compareOneLevel(const QString &path1, const QString &path2);

protected:
	QString directoryPath1_;
	QString directoryPath2_;

	QStringList uniqueSide1Files_;
	QStringList uniqueSide2Files_;
	QStringList uniqueSide1Directories_;
	QStringList uniqueSide2Directories_;
};

#endif // AMRECURSIVEDIRECTORYCOMPARE_H
