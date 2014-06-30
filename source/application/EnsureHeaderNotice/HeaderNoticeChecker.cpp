#include "HeaderNoticeChecker.h"

#include <QDebug>
#include <QProcess>

HeaderNoticeChecker::HeaderNoticeChecker(const QString &oldNotice, const QString &newNotice, const QString &directoryPath)
{
	oldNotice_ = oldNotice;
	newNotice_ = newNotice;
	anyNotice_ = "This file is part of the Acquaman Data Acquisition and Management framework (\\\"Acquaman\\\").";
	fullNotice_ = "This file is part of the Acquaman Data Acquisition and Management framework (\\\"Acquaman\\\").\nAcquaman is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nAcquaman is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with Acquaman.  If not, see <http://www.gnu.org/licenses/>.\n";

	ignoreDirectories_ << "muParser" << "qjson" << "Eigen";

	QDir sourceDirectory(directoryPath);
	recurseDirectories(sourceDirectory.path(), sourceDirectory.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));

	//qDebug() << "Updated: " << oldToNew_;
	//qDebug() << "Newly added to: " << nothingToNew_;
	qDebug() << "Updated: ";
	for(int x = 0, size = oldToNew_.count(); x < size; x++)
		qDebug() << oldToNew_.at(x);
	qDebug() << "Newly added to: ";
	for(int x = 0, size = nothingToNew_.count(); x < size; x++)
		qDebug() << nothingToNew_.at(x);
}

void HeaderNoticeChecker::recurseDirectories(const QString &currentPath, const QStringList &directories){

	QProcess findOldHeaders;
	QStringList arguments;
	arguments << "-c" << "grep \""+oldNotice_+"\" -l "+currentPath+"/*.h";

	findOldHeaders.start("/bin/sh", arguments );
	if (!findOldHeaders.waitForStarted())
		return;

	if (!findOldHeaders.waitForFinished())
		return;

	QByteArray resultOld = findOldHeaders.readAll();
	QString resultOldAsString(resultOld);
	QStringList allHeadersWithOldNotice = resultOldAsString.split('\n');
	allHeadersWithOldNotice.removeLast();

	for(int x = 0; x < allHeadersWithOldNotice.count(); x++){
			QProcess fixText;
			QStringList arguments;
			//arguments << "-c" << "sed -i '' 's/"+oldNotice_+"/"+newNotice_+"/g' "+allHeadersWithOldNotice.at(x);
			arguments << "-c" << "perl -pi -e 's/"+oldNotice_+"/"+newNotice_+"/g' "+allHeadersWithOldNotice.at(x);

			fixText.start("/bin/sh", arguments );
			if (!fixText.waitForStarted())
				return;

			if (!fixText.waitForFinished())
				return;
			oldToNew_ << allHeadersWithOldNotice.at(x);
	}

	QProcess findOldCpps;
	arguments.clear();
	arguments << "-c" << "grep \""+oldNotice_+"\" -l "+currentPath+"/*.cpp";

	findOldCpps.start("/bin/sh", arguments );
	if (!findOldCpps.waitForStarted())
		return;

	if (!findOldCpps.waitForFinished())
		return;

	QByteArray resultOldCpp = findOldCpps.readAll();
	QString resultOldCppAsString(resultOldCpp);
	QStringList allCppsWithOldNotice = resultOldCppAsString.split('\n');
	allCppsWithOldNotice.removeLast();

	for(int x = 0; x < allCppsWithOldNotice.count(); x++){
			QProcess fixText;
			QStringList arguments;
			//arguments << "-c" << "sed -i '' 's/"+oldNotice_+"/"+newNotice_+"/g' "+allCppsWithOldNotice.at(x);
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
