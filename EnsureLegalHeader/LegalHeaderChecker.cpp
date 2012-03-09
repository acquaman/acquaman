#include "LegalHeaderChecker.h"

#include <QDebug>
#include <QProcess>

LegalHeaderChecker::LegalHeaderChecker()
{
	QDir sourceDirectory("/Users/fawkes/dev/acquaman/source");
	recurseDirectories(sourceDirectory.path(), sourceDirectory.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));
}

void LegalHeaderChecker::recurseDirectories(const QString &currentPath, const QStringList &directories){
	qDebug() << currentPath;
	qDebug() << directories;
	QDir thisDir(currentPath);
	/*
	QStringList fileFilters;
	fileFilters << "*.h";
	QStringList headerFiles = thisDir.entryList(fileFilters, QDir::Files | QDir::NoSymLinks);
	for(int x = 0; x < headerFiles.count(); x++){

	}
	*/
	QProcess findHeaders;
	QStringList arguments;
	arguments << "-c" << "grep \"Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.\" -l "+currentPath+"/*.h";
	qDebug() << arguments;
	findHeaders.start("/bin/sh", arguments );
	if (!findHeaders.waitForStarted())
		return;

	//findHeaders.write("Qt rocks!");
	//findHeaders.closeWriteChannel();

	if (!findHeaders.waitForFinished())
		return;

	QByteArray result = findHeaders.readAll();
	qDebug() << currentPath << result;

	for(int x = 0; x < directories.count(); x++){
		QDir nextDir(currentPath+"/"+directories.at(x));
		recurseDirectories(nextDir.path(), nextDir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));
	}
}
