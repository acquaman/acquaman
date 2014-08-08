#ifndef AMRECURSIVEDIRECTORYCOMPARE_H
#define AMRECURSIVEDIRECTORYCOMPARE_H

#include <QStringList>

class AMRecursiveDirectoryCompare
{
public:
	/// enum that describes the possible results of a comparison
	enum DirectoryCompareResult{
		FullyMatchingResult = 0,
		Side1ModifiedResult = 1,
		Side2ModifiedResult = 2,
		BothSidesModifiedResult = 3,
		UnableToDetermineResult  = 4,
		NeitherSideExistsResult = 5,
		Side1DoesNotExistResult = 6,
		Side2DoesNotExistResult = 7,
		BothSidesAreFiles = 8,
		Side1IsFile = 9,
		Side2IsFile = 10,
		NotYetRun = 11,
		InvalidResult
	};

	/// Constructor for the comparison object. Make sure that directoryPath1 and directoryPath2 are absolute file paths.
	AMRecursiveDirectoryCompare(const QString &directoryPath1, const QString &directoryPath2, const QStringList &excludePatterns = QStringList(), const QString &side1Name = "Side 1", const QString & side2Name = "Side 2");

	/// Runs a comparison between the two directories
	AMRecursiveDirectoryCompare::DirectoryCompareResult compare();

	/// Lists all of the unique side 1 files
	QStringList uniqueSide1Files() const;
	/// Lists all of the unique side 2 files
	QStringList uniqueSide2Files() const;
	/// Lists all of the unique side 1 directories
	QStringList uniqueSide1Directories() const;
	/// Lists all of the unique side 2 directories
	QStringList uniqueSide2Directories() const;
	/// Lists all of the files on side 1 that are newer
	QStringList newerSide1Files() const;
	/// Lists all of the files on side 2 that are newer
	QStringList newerSide2Files() const;
	/// Lists all of the files that don't fall into the other categories above
	QStringList unknownFiles() const;

protected:
	/// Recursive call to compare one level of the directory structure
	bool compareOneLevel(const QString &path1, const QString &path2);

protected:
	/// Path to directory 1
	QString directoryPath1_;
	/// Path to directory 2
	QString directoryPath2_;

	/// Gives a human-readable name to side 1
	QString side1Name_;
	/// Gives a human-readable name to side 2
	QString side2Name_;

	/// All of the unique files on side 1
	QStringList uniqueSide1Files_;
	/// All of the unique files on side 2
	QStringList uniqueSide2Files_;
	/// All of the unique directories on side 1
	QStringList uniqueSide1Directories_;
	/// All of the unique directories on side 1
	QStringList uniqueSide2Directories_;

	/// All of the newer files on side 1
	QStringList newerSide1Files_;
	/// All of the newer files on side 2
	QStringList newerSide2Files_;

	/// All of the files that don't fall into the above categories
	QStringList unknownFiles_;

	/// Holds a list of patterns that will be excluded from the comparison, for both files and directories
	QStringList excludePatterns_;
	/// List of the regular expressions generated from the excludedPatterns list
	QList<QRegExp> excludedRegExps_;
};

#endif // AMRECURSIVEDIRECTORYCOMPARE_H
