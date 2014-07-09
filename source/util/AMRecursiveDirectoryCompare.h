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
		UnableToDetermineResult  = 4,
		InvalidResult
	};

	AMRecursiveDirectoryCompare(const QString &directoryPath1, const QString &directoryPath2, const QString &side1Name = "Side 1", const QString & side2Name = "Side 2");

	AMRecursiveDirectoryCompare::DirectoryCompareResult compare();

	QStringList uniqueSide1Files() const;
	QStringList uniqueSide2Files() const;
	QStringList uniqueSide1Directories() const;
	QStringList uniqueSide2Directories() const;
	QStringList newerSide1Files() const;
	QStringList newerSide2Files() const;
	QStringList unknownFiles() const;

protected:
	bool compareOneLevel(const QString &path1, const QString &path2);

protected:
	QString directoryPath1_;
	QString directoryPath2_;

	QString side1Name_;
	QString side2Name_;

	QStringList uniqueSide1Files_;
	QStringList uniqueSide2Files_;
	QStringList uniqueSide1Directories_;
	QStringList uniqueSide2Directories_;

	QStringList newerSide1Files_;
	QStringList newerSide2Files_;

	QStringList unknownFiles_;
};

#endif // AMRECURSIVEDIRECTORYCOMPARE_H
