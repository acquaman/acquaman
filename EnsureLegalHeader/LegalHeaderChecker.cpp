#include "LegalHeaderChecker.h"

#include <QDebug>
#include <QProcess>

LegalHeaderChecker::LegalHeaderChecker(const QString &oldNotice, const QString &newNotice)
{
	oldNotice_ = oldNotice;
	newNotice_ = newNotice;
	anyNotice_ = "This file is part of the Acquaman Data Acquisition and Management framework (\\\"Acquaman\\\").";
	fullNotice_ = "\n\nThis file is part of the Acquaman Data Acquisition and Management framework (\"Acquaman\").\nAcquaman is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nAcquaman is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with Acquaman.  If not, see <http://www.gnu.org/licenses/>.\n";
	QDir sourceDirectory("/Users/fawkes/dev/acquaman/source");
	recurseDirectories(sourceDirectory.path(), sourceDirectory.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));
}

void LegalHeaderChecker::recurseDirectories(const QString &currentPath, const QStringList &directories){

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
		if(allHeadersWithOldNotice.at(x) == "/Users/fawkes/dev/acquaman/source/acquaman.h"){
			QProcess fixText;
			QStringList arguments;
			arguments << "-c" << "sed -i '' 's/"+oldNotice_+"/"+newNotice_+"/g' "+allHeadersWithOldNotice.at(x);
			qDebug() << "Trying " << arguments;
			fixText.start("/bin/sh", arguments );
			if (!fixText.waitForStarted())
				return;

			if (!fixText.waitForFinished())
				return;
			oldToNew_ << allHeadersWithOldNotice.at(x);
		}
	}

	QProcess findNoHeaders;
	arguments.clear();
	arguments << "-c" << "grep -L \""+anyNotice_+"\" "+currentPath+"/*.h";
	qDebug() << arguments;

	findNoHeaders.start("/bin/sh", arguments );
	if (!findNoHeaders.waitForStarted())
		return;

	if (!findNoHeaders.waitForFinished())
		return;

	QByteArray resultNone = findNoHeaders.readAll();
	QString resultNoneAsString(resultNone);
	QStringList allHeadersWithNoNotice = resultNoneAsString.split('\n');
	allHeadersWithNoNotice.removeLast();

	qDebug() << allHeadersWithNoNotice;
	for(int x = 0; x < allHeadersWithNoNotice.count(); x++){
		if(allHeadersWithNoNotice.at(x) == "/Users/fawkes/dev/acquaman/source/acquaman/AM2DDacqScanController.h"){
			/*
			QProcess fixText;
			QStringList arguments;
			arguments << "-c" << "sed -i '' 's/"+oldNotice_+"/"+newNotice_+"/g' "+allHeadersWithOldNotice.at(x);
			qDebug() << "Trying " << arguments;
			fixText.start("/bin/sh", arguments );
			if (!fixText.waitForStarted())
				return;

			if (!fixText.waitForFinished())
				return;
			oldToNew_ << allHeadersWithOldNotice.at(x);
			*/
		}
	}


	for(int x = 0; x < directories.count(); x++){
		QDir nextDir(currentPath+"/"+directories.at(x));
		recurseDirectories(nextDir.path(), nextDir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot));
	}
}
