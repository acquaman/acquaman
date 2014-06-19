#include "AMRecursiveDirectoryCompare.h"

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QDateTime>
#include <QCoreApplication>

#include <QDebug>

AMRecursiveDirectoryCompare::AMRecursiveDirectoryCompare(const QString &directoryPath1, const QString &directoryPath2)
{
	directoryPath1_ = directoryPath1;
	directoryPath2_ = directoryPath2;
}

AMRecursiveDirectoryCompare::DirectoryCompareResult AMRecursiveDirectoryCompare::compare(){
	compareOneLevel(directoryPath1_, directoryPath2_);
	qDebug() << "Done the compare";

	if(uniqueSide1Directories_.isEmpty() && uniqueSide1Files_.isEmpty() && uniqueSide2Directories_.isEmpty() && uniqueSide2Files_.isEmpty()){
		qDebug() << "The directories match entirely";
		return AMRecursiveDirectoryCompare::FullyMatchingResult;
	}
	else if(uniqueSide2Directories_.isEmpty() && uniqueSide2Files_.isEmpty()){
		qDebug() << "Side 1 has been modified, but not side 2";

		qDebug() << "Unique side 1 directories: " << uniqueSide1Directories_;
		qDebug() << "Unique side 1 files: " << uniqueSide1Files_;

		return AMRecursiveDirectoryCompare::Side1ModifiedResult;

	}
	else if(uniqueSide1Directories_.isEmpty() && uniqueSide1Files_.isEmpty()){
		qDebug() << "Side 2 has been modified, but not side 1";

		qDebug() << "Unique side 2 directories: " << uniqueSide2Directories_;
		qDebug() << "Unique side 2 files: " << uniqueSide2Files_;

		return AMRecursiveDirectoryCompare::Side2ModifiedResult;
	}
	else{
		qDebug() << "Both sides have been modified";

		qDebug() << "Unique side 1 directories: " << uniqueSide1Directories_;
		qDebug() << "Unique side 1 files: " << uniqueSide1Files_;

		qDebug() << "Unique side 2 directories: " << uniqueSide2Directories_;
		qDebug() << "Unique side 2 files: " << uniqueSide2Files_;

		return AMRecursiveDirectoryCompare::BothSidesModifiedResult;
	}
}

QStringList AMRecursiveDirectoryCompare::uniqueSide1Files() const
{
	return uniqueSide1Files_;
}

QStringList AMRecursiveDirectoryCompare::uniqueSide2Files() const
{
	return uniqueSide2Files_;
}

QStringList AMRecursiveDirectoryCompare::uniqueSide1Directories() const
{
	return uniqueSide1Directories_;
}

QStringList AMRecursiveDirectoryCompare::uniqueSide2Directories() const
{
	return uniqueSide2Directories_;
}

bool AMRecursiveDirectoryCompare::compareOneLevel(const QString &path1, const QString &path2){
	QDir directory1(path1);
	QDir directory2(path2);
	directory1.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
	directory2.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

	// Get a list of all direct subdirectories of this directory, and files it contains
	QFileInfoList directoryList1 = directory1.entryInfoList();
	QFileInfoList directoryList2 = directory2.entryInfoList();


	QHash<QString, QFileInfo> side1Files;
	QHash<QString, QFileInfo> side2Files;
	QStringList side1Directories;
	QStringList side2Directories;
	QStringList dualSideDirectories;
	QStringList onlySide1Directories;
	QStringList onlySide2Directories;

	// Builds side 1 lists: adds files to file hash, appends directories to directory list
	for(int x = 0, size = directoryList1.count(); x < size; x++)
	{
		if(directoryList1.at(x).isFile())
		{
			side1Files.insert(directoryList1.at(x).absoluteFilePath().remove(path1), directoryList1.at(x));
		}
		else if(directoryList1.at(x).isDir())
		{
			side1Directories.append(directoryList1.at(x).absoluteFilePath().remove(path1));
		}
	}

	// Builds side 2 lists, while removing files it finds in both hashes from the list 1
	// hash
	for(int x = 0, size = directoryList2.count(); x < size; x++){
		if(directoryList2.at(x).isFile()){
			if(side1Files.contains(directoryList2.at(x).absoluteFilePath().remove(path2)))
				side1Files.remove(directoryList2.at(x).absoluteFilePath().remove(path2));
			else
				side2Files.insert(directoryList2.at(x).absoluteFilePath().remove(path2), directoryList2.at(x));
		}
		else if(directoryList2.at(x).isDir()){
			side2Directories.append(directoryList2.at(x).absoluteFilePath().remove(path2));
		}
	}

	// Locates directories in their correct list
	for(int x = 0, size = side2Directories.count(); x < size; x++){
		if(side1Directories.contains(side2Directories.at(x))){
			dualSideDirectories.append(side2Directories.at(x));
			side1Directories.removeAll(side2Directories.at(x));
		}
		else
			onlySide2Directories.append(side2Directories.at(x));
	}
	onlySide1Directories = side1Directories;

	for(int x = 0, size = onlySide1Directories.count(); x < size; x++)
		uniqueSide1Directories_.append(QDir::cleanPath(QString("%1/%2").arg(path1).arg(onlySide1Directories.at(x))));
	for(int x = 0, size = onlySide2Directories.count(); x < size; x++)
		uniqueSide2Directories_.append(QDir::cleanPath(QString("%1/%2").arg(path2).arg(onlySide2Directories.at(x))));

	if(side1Files.count() > 0){
		QHash<QString, QFileInfo>::const_iterator i = side1Files.constBegin();
		while (i != side1Files.constEnd()) {
			uniqueSide1Files_.append(QDir::cleanPath(QString("%1/%2").arg(path1).arg(i.key())));
			++i;
		}
	}

	if(side2Files.count() > 0){
		QHash<QString, QFileInfo>::const_iterator j = side2Files.constBegin();
		while (j != side2Files.constEnd()) {
			uniqueSide2Files_.append(QDir::cleanPath(QString("%1/%2").arg(path2).arg(j.key())));
			++j;
		}
	}


	for(int x = 0, size = dualSideDirectories.count(); x < size; x++){
		compareOneLevel(QDir::cleanPath(QString("%1/%2").arg(path1).arg(dualSideDirectories.at(x))), QDir::cleanPath(QString("%1/%2").arg(path2).arg(dualSideDirectories.at(x))));
	}



	return false;
}
