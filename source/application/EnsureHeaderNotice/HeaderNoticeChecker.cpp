/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "HeaderNoticeChecker.h"

#include <QDebug>
#include <QProcess>

HeaderNoticeChecker::HeaderNoticeChecker(const QString &oldNotice, const QString &newNotice, const QString &directoryPath)
{
	oldNotice_ = oldNotice;
	newNotice_ = newNotice;
	anyNotice_ = "This file is part of the Acquaman Data Acquisition and Management framework (\\\"Acquaman\\\").";
	fullNotice_ = "\n\nThis file is part of the Acquaman Data Acquisition and Management framework (\\\"Acquaman\\\").\n\nAcquaman is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nAcquaman is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with Acquaman.  If not, see <http://www.gnu.org/licenses/>.\n";

	ignoreDirectories_ << "muParser" << "qjson" << "Eigen" << "EnsureHeaderNotice";

	QDir sourceDirectory(directoryPath);
	recurseDirectories(sourceDirectory.path(), sourceDirectory.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));

	qDebug() << "Updated: ";
	for(int x = 0, size = oldToNew_.count(); x < size; x++)
		qDebug() << oldToNew_.at(x);
	qDebug() << "Newly added to: ";
	for(int x = 0, size = nothingToNew_.count(); x < size; x++)
		qDebug() << nothingToNew_.at(x);
}

void HeaderNoticeChecker::recurseDirectories(const QString &currentPath, const QStringList &directories){

	QStringList arguments;

	QStringList allHeadersWithOldNotice;
	QDir currentDirectory(currentPath);
	QStringList onlyHeadersFilter;
	onlyHeadersFilter << "*.h";
	QFileInfoList headerEntries = currentDirectory.entryInfoList(onlyHeadersFilter, QDir::Files);
	QString fileText;
	for(int x = 0, size = headerEntries.size(); x < size; x++){
		fileText.clear();
		QFile readFile(headerEntries.at(x).absoluteFilePath());
		if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		QTextStream fileIn(&readFile);
		fileText = fileIn.readAll();

		if(fileText.contains(oldNotice_) && !fileText.contains(newNotice_))
			allHeadersWithOldNotice.append(headerEntries.at(x).absoluteFilePath());
	}


	for(int x = 0; x < allHeadersWithOldNotice.count(); x++){
			QProcess fixText;
			QStringList arguments;
			arguments << "-c" << "perl -pi -e 's/"+oldNotice_+"/"+newNotice_+"/g' "+allHeadersWithOldNotice.at(x);

			fixText.start("/bin/sh", arguments );
			if (!fixText.waitForStarted())
				return;

			if (!fixText.waitForFinished())
				return;
			oldToNew_ << allHeadersWithOldNotice.at(x);
	}

	QStringList allCppsWithOldNotice;
	QStringList onlyCppsFilter;
	onlyCppsFilter << "*.cpp";
	QFileInfoList cppEntries = currentDirectory.entryInfoList(onlyCppsFilter, QDir::Files);
	for(int x = 0, size = cppEntries.size(); x < size; x++){
		fileText.clear();
		QFile readFile(cppEntries.at(x).absoluteFilePath());
		if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		QTextStream fileIn(&readFile);
		fileText = fileIn.readAll();

		if(fileText.contains(oldNotice_) && fileText.contains(newNotice_))
			qDebug() << "Found a new notice in " << cppEntries.at(x).absoluteFilePath();

		if(fileText.contains(oldNotice_) && !fileText.contains(newNotice_))
			allCppsWithOldNotice.append(cppEntries.at(x).absoluteFilePath());
	}

	for(int x = 0; x < allCppsWithOldNotice.count(); x++){
			QProcess fixText;
			QStringList arguments;
			arguments << "-c" << "perl -pi -e 's/"+oldNotice_+"/"+newNotice_+"/g' "+allCppsWithOldNotice.at(x);
			fixText.start("/bin/sh", arguments );
			if (!fixText.waitForStarted())
				return;

			if (!fixText.waitForFinished())
				return;
			oldToNew_ << allCppsWithOldNotice.at(x);
	}

	QProcess findNoHeaders;
	arguments.clear();
	arguments << "-c" << "grep -L \""+anyNotice_+"\" "+currentPath+"/*.h";

	findNoHeaders.start("/bin/sh", arguments );
	if (!findNoHeaders.waitForStarted())
		return;

	if (!findNoHeaders.waitForFinished())
		return;

	QByteArray resultNone = findNoHeaders.readAll();
	QString resultNoneAsString(resultNone);
	QStringList allHeadersWithNoNotice = resultNoneAsString.split('\n');
	allHeadersWithNoNotice.removeLast();

	for(int x = 0; x < allHeadersWithNoNotice.count(); x++){
			QProcess addText;
			QStringList arguments;
			arguments << "-c" << "perl -pi -e 'print \"/*\n"+newNotice_+fullNotice_+"*/\n\n\n\" if $. == 1' "+allHeadersWithNoNotice.at(x);
			addText.start("/bin/sh", arguments );
			if (!addText.waitForStarted())
				return;

			if (!addText.waitForFinished())
				return;
			nothingToNew_ << allHeadersWithNoNotice.at(x);
	}

	QProcess findNoCpps;
	arguments.clear();
	arguments << "-c" << "grep -L \""+anyNotice_+"\" "+currentPath+"/*.cpp";

	findNoCpps.start("/bin/sh", arguments );
	if (!findNoCpps.waitForStarted())
		return;

	if (!findNoCpps.waitForFinished())
		return;

	QByteArray resultNoneCpp = findNoCpps.readAll();
	QString resultNoneCppAsString(resultNoneCpp);
	QStringList allCppsWithNoNotice = resultNoneCppAsString.split('\n');
	allCppsWithNoNotice.removeLast();

	for(int x = 0; x < allCppsWithNoNotice.count(); x++){
			QProcess addText;
			QStringList arguments;
			arguments << "-c" << "perl -pi -e 'print \"/*\n"+newNotice_+fullNotice_+"*/\n\n\n\" if $. == 1' "+allCppsWithNoNotice.at(x);
			addText.start("/bin/sh", arguments );
			if (!addText.waitForStarted())
				return;

			if (!addText.waitForFinished())
				return;
			nothingToNew_ << allCppsWithNoNotice.at(x);
	}

	for(int x = 0; x < directories.count(); x++){
		if(!ignoreDirectories_.contains(directories.at(x))){
			QDir nextDir(currentPath+"/"+directories.at(x));
			recurseDirectories(nextDir.path(), nextDir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));
		}
		else
			qDebug() << "Ignoring " << currentPath+"/"+directories.at(x);
	}
}
