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

	if(uniqueSide1Directories_.isEmpty() && uniqueSide1Files_.isEmpty() &&
	   uniqueSide2Directories_.isEmpty() && uniqueSide2Files_.isEmpty() &&
	   newerSide1Files_.isEmpty() && newerSide2Files().isEmpty())
	{
		qDebug() << "The directories match entirely";
		return AMRecursiveDirectoryCompare::FullyMatchingResult;
	}
	else if(uniqueSide2Directories_.isEmpty() && uniqueSide2Files_.isEmpty() &&
			newerSide2Files_.isEmpty())
	{
		qDebug() << "Side 1 has been modified, but not side 2";

		qDebug() << "Unique side 1 directories: " << uniqueSide1Directories_;
		qDebug() << "Unique side 1 files: " << uniqueSide1Files_;
		qDebug() << "Newer side 1 files: " << newerSide1Files_;

		return AMRecursiveDirectoryCompare::Side1ModifiedResult;

	}
	else if(uniqueSide1Directories_.isEmpty() && uniqueSide1Files_.isEmpty() &&
			newerSide1Files_.isEmpty())
	{
		qDebug() << "Side 2 has been modified, but not side 1";

		qDebug() << "Unique side 2 directories: " << uniqueSide2Directories_;
		qDebug() << "Unique side 2 files: " << uniqueSide2Files_;
		qDebug() << "Newer side 2 files: " << newerSide2Files_;

		return AMRecursiveDirectoryCompare::Side2ModifiedResult;
	}
	else{
		qDebug() << "Both sides have been modified";

		qDebug() << "Unique side 1 directories: " << uniqueSide1Directories_;
		qDebug() << "Unique side 1 files: " << uniqueSide1Files_;		
		qDebug() << "Newer side 1 files: " << newerSide1Files_;
		qDebug() << "=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+";
		qDebug() << "Unique side 2 directories: " << uniqueSide2Directories_;
		qDebug() << "Unique side 2 files: " << uniqueSide2Files_;		
		qDebug() << "Newer side 2 files: " << newerSide2Files_;

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

QStringList AMRecursiveDirectoryCompare::newerSide1Files() const
{
	return newerSide1Files_;
}

QStringList AMRecursiveDirectoryCompare::newerSide2Files() const
{
	return newerSide2Files_;
}

bool AMRecursiveDirectoryCompare::compareOneLevel(const QString &path1, const QString &path2){
	QDir directory1(path1);
	QDir directory2(path2);
	directory1.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
	directory2.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

	// Get a list of all direct subdirectories of this directory, and files it contains
	QFileInfoList directory1Entries = directory1.entryInfoList();
	QFileInfoList directory2Entries = directory2.entryInfoList();


	QHash<QString, QFileInfo> side1Files;
	QHash<QString, QFileInfo> side2Files;
	QStringList side1SubDirectories;
	QStringList side2SubDirectories;
	QStringList dualSideSubDirectories;
	QStringList onlySide1SubDirectories;
	QStringList onlySide2SubDirectories;
	QStringList newerSide1Files;
	QStringList newerSide2Files;

	// Builds side 1 lists: adds files to file hash, appends directories to directory list
	for(int x = 0, size = directory1Entries.count(); x < size; x++)
	{
		if(directory1Entries.at(x).isFile())
		{
			side1Files.insert(directory1Entries.at(x).absoluteFilePath().remove(path1), directory1Entries.at(x));
		}
		else if(directory1Entries.at(x).isDir())
		{
			side1SubDirectories.append(directory1Entries.at(x).absoluteFilePath().remove(path1));
		}
	}

	// Builds side 2 lists, while removing files it finds in both hashes from the list 1
	// hash
	for(int x = 0, size = directory2Entries.count(); x < size; x++){
		QFileInfo currentSide2Entry = directory2Entries.at(x);
		if(currentSide2Entry.isFile())
		{
			if(side1Files.contains(currentSide2Entry.absoluteFilePath().remove(path2)))
			{
				QFileInfo side1Match = side1Files.value(currentSide2Entry.absoluteFilePath().remove(path2));
				if(side1Match.lastModified() < currentSide2Entry.lastModified())
					newerSide2Files.append(currentSide2Entry.absoluteFilePath().remove(path2));
				else if(currentSide2Entry.lastModified() < side1Match.lastModified())
					newerSide1Files.append(side1Match.absoluteFilePath().remove(path1));
				side1Files.remove(currentSide2Entry.absoluteFilePath().remove(path2));
			}
			else
			{
				side2Files.insert(currentSide2Entry.absoluteFilePath().remove(path2), currentSide2Entry);
			}


		}
		else if(directory2Entries.at(x).isDir()){
			side2SubDirectories.append(directory2Entries.at(x).absoluteFilePath().remove(path2));
		}
	}

	// Locates directories in their correct list
	for(int x = 0, size = side2SubDirectories.count(); x < size; x++){
		if(side1SubDirectories.contains(side2SubDirectories.at(x))){
			dualSideSubDirectories.append(side2SubDirectories.at(x));
			side1SubDirectories.removeAll(side2SubDirectories.at(x));
		}
		else
			onlySide2SubDirectories.append(side2SubDirectories.at(x));
	}
	onlySide1SubDirectories = side1SubDirectories;

	for(int x = 0, size = newerSide1Files.count(); x < size; x++){
		qDebug() << "Found newer files on side 1";
		newerSide1Files_.append(QDir::cleanPath(QString("%1/%2").arg(path1).arg(newerSide1Files.at(x))));
	}
	for(int x = 0, size = newerSide2Files.count(); x < size; x++){
		qDebug() << "Found newer files on side 2";
		newerSide2Files_.append(QDir::cleanPath(QString("%1/%2").arg(path2).arg(newerSide2Files.at(x))));
	}
	for(int x = 0, size = onlySide1SubDirectories.count(); x < size; x++)
		uniqueSide1Directories_.append(QDir::cleanPath(QString("%1/%2").arg(path1).arg(onlySide1SubDirectories.at(x))));
	for(int x = 0, size = onlySide2SubDirectories.count(); x < size; x++)
		uniqueSide2Directories_.append(QDir::cleanPath(QString("%1/%2").arg(path2).arg(onlySide2SubDirectories.at(x))));

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


	for(int x = 0, size = dualSideSubDirectories.count(); x < size; x++){
		compareOneLevel(QDir::cleanPath(QString("%1/%2").arg(path1).arg(dualSideSubDirectories.at(x))), QDir::cleanPath(QString("%1/%2").arg(path2).arg(dualSideSubDirectories.at(x))));
	}



	return false;
}
