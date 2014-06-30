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


#include "AMDeploy.h"

#include <QProcess>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include <QDebug>

AMDeploy::AMDeploy(const QString &projectFile, const QString &workingDirectory, QObject *parent) :
	QObject(parent)
{
	workingDirectory_ = workingDirectory;
	projectFile_ = projectFile;
	applicationArguments_ = QCoreApplication::instance()->arguments();

	bool foundExclusion = false;
	QFile gitExcludeFile(QString("%1/.git/info/exclude").arg(workingDirectory_));
	if (!gitExcludeFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream gitExcludeIn(&gitExcludeFile);
	while (!gitExcludeIn.atEnd()) {
		QString line = gitExcludeIn.readLine();
		if(line.contains("source/util/AMRunTimeBuildInfo.*"))
			foundExclusion = true;
	}
	gitExcludeFile.close();

	if(!foundExclusion){
		QFile gitExcludeFileWrite(QString("%1/.git/info/exclude").arg(workingDirectory_));
		if (!gitExcludeFileWrite.open(QIODevice::Append | QIODevice::Text))
			return;

		QTextStream gitExcludeOut(&gitExcludeFileWrite);
		gitExcludeOut << "source/util/AMRunTimeBuildInfo.*\n";
		gitExcludeFileWrite.close();
	}

	startDomainNameProcess();;
}

void AMDeploy::startDomainNameProcess(){
	domainNameProcess_ = new QProcess();
	QString program = "domainname";
	QStringList arguments;
	domainNameProcess_->setWorkingDirectory(workingDirectory_);

	connect(domainNameProcess_, SIGNAL(readyRead()), this, SLOT(onDomainNameProcessReadReady()));
	connect(domainNameProcess_, SIGNAL(finished(int)), this, SLOT(onDomainNameProcesFinished(int)));
	domainNameProcess_->start(program, arguments);
}

void AMDeploy::onDomainNameProcessReadReady(){
	domainNameOutput_.append(domainNameProcess_->readAllStandardOutput());
}

void AMDeploy::onDomainNameProcesFinished(int status){
	if(status != 0){
		qDebug() << QString("Cannot continue with deployment, domainname exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		qDebug() << "domainname reported " << domainNameOutput_;

		if(domainNameOutput_.contains(".clsi.ca")){
			QString currentPATH = getenv(QString("PATH").toAscii().data());
			QString updatedPATH = QString("%1:%2:%3").arg("/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/bin:/home/beamline/tools/gstreamer-0.10.35/deps/bin").arg("/home/beamline/tools/qt/qt-4.7.3-SL54-x86_64/bin").arg(currentPATH);

			QString currentLDLIBRARYPATH = getenv(QString("LD_LIBRARY_PATH").toAscii().data());
			QString updatedLDLIBRARYPATH = QString("%1:%2:%3").arg("/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib:/home/beamline/tools/gstreamer-0.10.35/deps/lib").arg("/home/beamline/tools/qt/qt-4.7.3-SL54-x86_64/lib:/home/beamline/tools/qt/qt-mobility-1.1.3-SL54-x86_64/lib").arg(currentLDLIBRARYPATH);

			QString currentQTPLUGINPATH = getenv(QString("QT_PLUGIN_PATH").toAscii().data());
			QString updatedQTPLUGINPATH = QString("%1:%2").arg("/home/beamline/tools/qt/qt-mobility-1.1.3-SL54-x86_64/plugins").arg(currentQTPLUGINPATH);

			setenv(QString("PATH").toAscii().data(), updatedPATH.toAscii().data(), 1);
			setenv(QString("LD_LIBRARY_PATH").toAscii().data(), updatedLDLIBRARYPATH.toAscii().data(), 1);
			setenv(QString("QT_PLUGIN_PATH").toAscii().data(), updatedQTPLUGINPATH.toAscii().data(), 1);

			QString newPATH = getenv(QString("PATH").toAscii().data());
			QString newLDLIBRARYPATH = getenv(QString("LD_LIBRARY_PATH").toAscii().data());
			QString newQTPLUGINPATH = getenv(QString("QT_PLUGIN_PATH").toAscii().data());

			if(updatedPATH != newPATH){
				qDebug() << "Attempt to update PATH to " << updatedPATH << " failed. Cannot continue";
				QCoreApplication::instance()->exit(-1);
			}
			else if(updatedLDLIBRARYPATH != newLDLIBRARYPATH){
				qDebug() << "Attempt to update LD_LIBRARY_PATH to " << updatedLDLIBRARYPATH << " failed. Cannot continue";
				QCoreApplication::instance()->exit(-1);
			}
			else if(updatedQTPLUGINPATH != newQTPLUGINPATH){
				qDebug() << "Attempt to update QT_PLUGIN_PATH to " << updatedQTPLUGINPATH << " failed. Cannot continue";
				QCoreApplication::instance()->exit(-1);
			}
		}

		if(applicationArguments_.contains("--clsMake")){
			qDebug() << "Detected --clsMake, skipping to qmake steps.";
			startQMakeVersionProcess();
		}
		else
			startGitStatusProcess();
	}
}

void AMDeploy::startGitStatusProcess(){
	gitStatusProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "status" << "--short";
	gitStatusProcess_->setWorkingDirectory(workingDirectory_);

	connect(gitStatusProcess_, SIGNAL(readyRead()), this, SLOT(onGitStatusProcessReadReady()));
	connect(gitStatusProcess_, SIGNAL(finished(int)), this, SLOT(onGitStatusProcessFinished(int)));
	gitStatusProcess_->start(program, arguments);
}

void AMDeploy::onGitStatusProcessReadReady(){
	gitStatusOutput_.append(gitStatusProcess_->readAllStandardOutput());
//	qDebug() << "Git status says: " << gitStatusOutput_;
}

void AMDeploy::onGitStatusProcessFinished(int status){
//	qDebug() << "Git status exited with " << status;

	if(status != 0){
		qDebug() << QString("Cannot continue with deployment, git status exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else if(status == 0 && gitStatusOutput_.isEmpty()){
		startGitBranchProcess();
	}
	else{
		qDebug() << "Cannot continue with deployment, there are uncommitted changes present.";
		QCoreApplication::instance()->exit(-1);
	}
}

void AMDeploy::startGitBranchProcess(){
	gitBranchProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "branch";
	gitBranchProcess_->setWorkingDirectory(workingDirectory_);

	connect(gitBranchProcess_, SIGNAL(readyRead()), this, SLOT(onGitBranchProcessReadReady()));
	connect(gitBranchProcess_, SIGNAL(finished(int)), this, SLOT(onGitBranchProcessFinished(int)));
	gitBranchProcess_->start(program, arguments);
}

void AMDeploy::onGitBranchProcessReadReady(){
	gitBranchOutput_.append(gitBranchProcess_->readAllStandardOutput());
//	qDebug() << "Git branch says: " << gitBranchOutput_;
}

void AMDeploy::onGitBranchProcessFinished(int status){
//	qDebug() << "Git branch exited with " << status;
	if(status != 0){
		qDebug() << QString("Cannot continue with deployment, git branch exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		QStringList branches = gitBranchOutput_.split("\n");
		QString currentBranch;
		for(int x = 0, size = branches.count(); x < size && currentBranch.isNull(); x++)
			if(branches.at(x).startsWith("* "))
				currentBranch = branches.at(x);

		if(!currentBranch.isNull())
			currentBranch.remove("* ");

//		qDebug() << "The current branch is " << currentBranch;
		currentBranch_ = currentBranch;

		startGitLogProcess();
	}
}

void AMDeploy::startGitLogProcess(){
	gitLogProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "log" << "-n" << "1";
	gitLogProcess_->setWorkingDirectory(workingDirectory_);

	connect(gitLogProcess_, SIGNAL(readyRead()), this, SLOT(onGitLogProcessReadReady()));
	connect(gitLogProcess_, SIGNAL(finished(int)), this, SLOT(onGitLogProcessFinished(int)));
	gitLogProcess_->start(program, arguments);
}

void AMDeploy::onGitLogProcessReadReady(){
	gitLogOutput_.append(gitLogProcess_->readAllStandardOutput());
//	qDebug() << "Git log says: " << gitLogOutput_;
}

void AMDeploy::onGitLogProcessFinished(int status){
//	qDebug() << "Git log exited with " << status;
	if(status != 0){
		qDebug() << QString("Cannot continue with deployment, git log exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		QStringList logLines = gitLogOutput_.split("\n");
		QString commitString;
		QString authorString;
		QString dateString;
		for(int x = 0, size = logLines.count(); x < size && (commitString.isNull() || authorString.isNull() || dateString.isNull()); x++){
			if(logLines.at(x).contains("commit ")){
				commitString = logLines.at(x);
				commitString.remove("commit ");
				commitString = commitString.simplified();
			}
			else if(logLines.at(x).contains("Author: ")){
				authorString = logLines.at(x);
				authorString.remove("Author: ");
				authorString = authorString.simplified();
			}
			else if(logLines.at(x).contains("Date: ")){
				dateString = logLines.at(x);
				dateString.remove("Date: ");
				dateString = dateString.simplified();
			}
		}

//		qDebug() << "Commit is " << commitString;
//		qDebug() << "Author is " << authorString;
//		qDebug() << "Date is " << dateString;

		currentCommitSHA_ = commitString;
		currentCommitAuthor_ = authorString;
		currentCommitDate_ = dateString;

		startGitDescribeProcess();
	}
}

void AMDeploy::startGitDescribeProcess(){
	gitDescribeProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "describe";
	gitDescribeProcess_->setWorkingDirectory(workingDirectory_);

	connect(gitDescribeProcess_, SIGNAL(readyRead()), this, SLOT(onGitDescribeProcessReadReady()));
	connect(gitDescribeProcess_, SIGNAL(finished(int)), this, SLOT(onGitDescribeProcessFinished(int)));
	gitDescribeProcess_->start(program, arguments);
}

void AMDeploy::onGitDescribeProcessReadReady(){
	gitDescribeOutput_.append(gitDescribeProcess_->readAllStandardOutput());
//	qDebug() << "Git describe says: " << gitDescribeOutput_;
	currentGitDescription_ = gitDescribeOutput_.trimmed();
}

void AMDeploy::onGitDescribeProcessFinished(int status){
//	qDebug() << "Git describe exited with " << status;
	if(status != 0){
		qDebug() << QString("Cannot continue with deployment, git describe exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		qDebug() << "Branch: " << currentBranch_;
		qDebug() << "Commit: " << currentCommitSHA_;
		qDebug() << "Author: " << currentCommitAuthor_;
		qDebug() << "Date: " << currentCommitDate_;
		qDebug() << "Describe: " << currentGitDescription_;

		QString runTimeBuildInfoHeader;
		QString runTimeBuildInfoSource;

		runTimeBuildInfoHeader.append("#ifndef AMRUNTIMEBUILDINFO_H\n");
		runTimeBuildInfoHeader.append("#define AMRUNTIMEBUILDINFO_H\n\n");
		runTimeBuildInfoHeader.append("#include \"source/util/AMBuildInfo.h\"\n\n");
		runTimeBuildInfoHeader.append("// WARNING: This file is automatically generated during the AMDeploy process, any changes you make will be lost\n");
		runTimeBuildInfoHeader.append("class AMRunTimeBuildInfo : public AMBuildInfo\n");
		runTimeBuildInfoHeader.append("{\n");
		runTimeBuildInfoHeader.append("public:\n");
		runTimeBuildInfoHeader.append("\tAMRunTimeBuildInfo();\n");
		runTimeBuildInfoHeader.append("};\n\n");
		runTimeBuildInfoHeader.append("#endif // AMBUILDINFO_H");

		runTimeBuildInfoSource.append("#include \"AMRunTimeBuildInfo.h\"\n\n");
		runTimeBuildInfoSource.append("AMRunTimeBuildInfo::AMRunTimeBuildInfo() :\n");
		runTimeBuildInfoSource.append("\tAMBuildInfo()\n");
		runTimeBuildInfoSource.append("{\n");
		runTimeBuildInfoSource.append(QString("\tsetCommitSHA(\"%1\");\n").arg(currentCommitSHA_));
		runTimeBuildInfoSource.append(QString("\tsetBranchName(\"%1\");\n").arg(currentBranch_));
		runTimeBuildInfoSource.append(QString("\tsetDescribeName(\"%1\");\n").arg(currentGitDescription_));
		runTimeBuildInfoSource.append(QString("\tsetTagName(\"%1\");\n").arg(currentGitDescription_.section('-', 0, 0)));
		runTimeBuildInfoSource.append(QString("\tsetLastCommitAuthor(\"%1\");\n").arg(currentCommitAuthor_));
		runTimeBuildInfoSource.append(QString("\tsetCommitDate(\"%1\");\n").arg(currentCommitDate_));
		runTimeBuildInfoSource.append("}\n");

		QFile headerFile(QString("%1/source/util/AMRunTimeBuildInfo.h").arg(workingDirectory_));
		if (!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream headerOut(&headerFile);
		headerOut << runTimeBuildInfoHeader;
		headerFile.close();

		QFile sourceFile(QString("%1/source/util/AMRunTimeBuildInfo.cpp").arg(workingDirectory_));
		if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream sourceOut(&sourceFile);
		sourceOut << runTimeBuildInfoSource;
		sourceFile.close();

		startQMakeVersionProcess();
	}
}

void AMDeploy::startQMakeVersionProcess(){
	qmakeVersionProcess_ = new QProcess();
	QString program = "qmake";
	QStringList arguments;
	arguments << "--version";
	qmakeVersionProcess_->setWorkingDirectory(workingDirectory_);

	connect(qmakeVersionProcess_, SIGNAL(readyRead()), this, SLOT(onQMakeVersionProcessReadReady()));
	connect(qmakeVersionProcess_, SIGNAL(finished(int)), this, SLOT(onQMakeVersionProcessFinished(int)));
	qmakeVersionProcess_->start(program, arguments);
}

void AMDeploy::onQMakeVersionProcessReadReady(){
	qmakeVersionOutput_ = qmakeVersionProcess_->readAllStandardOutput();
}

void AMDeploy::onQMakeVersionProcessFinished(int status){
	if(status != 0 || !qmakeVersionOutput_.contains("QMake version 2.")){
		qDebug() << QString("The qmake version %1 is not supported, must be qmake 2.x or better.").arg(qmakeVersionOutput_);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		if(QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMRunTimeBuildInfo.o")) || QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMBuildInfo.o")) || QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMBuildReporter.o")))
			startCleanupObjectFilesProcess();
		else
			startQMakeProcess();
	}
}

void AMDeploy::startCleanupObjectFilesProcess(){
	cleanupObjectFilesProcess_ = new QProcess();
	QString program = "rm";
	QStringList arguments;
	if(QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMRunTimeBuildInfo.o")))
		arguments << "AMRunTimeBuildInfo.o";
	if(QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMBuildInfo.o")))
		arguments << "AMBuildInfo.o";
	if(QFile::exists(QString("%1/%2").arg(workingDirectory_).arg("AMBuildReporter.o")))
		arguments << "AMBuildReporter.o";
	cleanupObjectFilesProcess_->setWorkingDirectory(workingDirectory_);

	connect(cleanupObjectFilesProcess_, SIGNAL(readyRead()), this, SLOT(onCleanupObjectFilesProcessReadReady()));
	connect(cleanupObjectFilesProcess_, SIGNAL(finished(int)), this, SLOT(onCleanupObjectFilesProcessFinished(int)));
	cleanupObjectFilesProcess_->start(program, arguments);
}

void AMDeploy::onCleanupObjectFilesProcessReadReady(){
}

void AMDeploy::onCleanupObjectFilesProcessFinished(int status){
	if(status != 0){
		qDebug() << QString("Failed to cleanup old buildInfo files, exit status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else
		startQMakeProcess();
}

void AMDeploy::startQMakeProcess(){
	qmakeProcess_ = new QProcess();
	QString program = "qmake";
	QStringList arguments;
	arguments << projectFile_ << "-r";
	#ifdef Q_WS_MAC
	arguments << "-spec" << "unsupported/macx-clang" << "CONFIG+=x86_64";
	#endif
	if(!applicationArguments_.contains("--clsMake"))
		arguments << "DEFINES+=AM_BUILD_REPORTER_ENABLED";
	qmakeProcess_->setWorkingDirectory(workingDirectory_);

	connect(qmakeProcess_, SIGNAL(readyRead()), this, SLOT(onQMakeProcessReadReady()));
	connect(qmakeProcess_, SIGNAL(finished(int)), this, SLOT(onQMakeProcessFinished(int)));
	qmakeProcess_->start(program, arguments);
}

void AMDeploy::onQMakeProcessReadReady(){
	qDebug() << qmakeProcess_->readAll();
}

void AMDeploy::onQMakeProcessFinished(int status){
	if(status != 0){
		qDebug() << "The qmake process failed with an exit status of " << status;
		QCoreApplication::instance()->exit(-1);
	}
	else
		startMakeProcess();
}

void AMDeploy::startMakeProcess(){
	makeProcess_ = new QProcess();
	QString program = "make";
	QStringList arguments;
	makeProcess_->setWorkingDirectory(workingDirectory_);

	connect(makeProcess_, SIGNAL(readyRead()), this, SLOT(onMakeProcessReadReady()));
	connect(makeProcess_, SIGNAL(finished(int)), this, SLOT(onMakeProcessFinished(int)));
	makeProcess_->start(program, arguments);
}

void AMDeploy::onMakeProcessReadReady(){
	qDebug() << makeProcess_->readAll();
}

void AMDeploy::onMakeProcessFinished(int status){
	if(status != 0){
		qDebug() << "The make process failed with an exit status of " << status;
		QCoreApplication::instance()->exit(-1);
	}
	else{
		QCoreApplication::instance()->quit();
	}
}

AMDeploy::~AMDeploy()
{
}
