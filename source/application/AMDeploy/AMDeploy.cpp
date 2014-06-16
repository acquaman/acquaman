#include "AMDeploy.h"

#include <QProcess>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include <QDebug>

AMDeploy::AMDeploy(const QString &workingDirectory, const QString &projectFile, QObject *parent) :
	QObject(parent)
{
	workingDirectory_ = workingDirectory;
	projectFile_ = projectFile;

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
	if(status == 0 && gitStatusOutput_.isEmpty()){
		gitBranchProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "branch";
		gitBranchProcess_->setWorkingDirectory(workingDirectory_);

		connect(gitBranchProcess_, SIGNAL(readyRead()), this, SLOT(onGitBranchProcessReadReady()));
		connect(gitBranchProcess_, SIGNAL(finished(int)), this, SLOT(onGitBranchProcessFinished(int)));
		gitBranchProcess_->start(program, arguments);
	}
	else if(status != 0){
		qDebug() << QString("Cannot continue with deployment, git status exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
	else{
		qDebug() << "Cannot continue with deployment, there are uncommitted changes present.";
		QCoreApplication::instance()->exit(-1);
	}
}

void AMDeploy::onGitBranchProcessReadReady(){
	gitBranchOutput_.append(gitBranchProcess_->readAllStandardOutput());
//	qDebug() << "Git branch says: " << gitBranchOutput_;
}

void AMDeploy::onGitBranchProcessFinished(int status){
//	qDebug() << "Git branch exited with " << status;
	if(status == 0){
		QStringList branches = gitBranchOutput_.split("\n");
		QString currentBranch;
		for(int x = 0, size = branches.count(); x < size && currentBranch.isNull(); x++)
			if(branches.at(x).startsWith("* "))
				currentBranch = branches.at(x);

		if(!currentBranch.isNull())
			currentBranch.remove("* ");

//		qDebug() << "The current branch is " << currentBranch;
		currentBranch_ = currentBranch;

		gitLogProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "log" << "-n" << "1";
		gitLogProcess_->setWorkingDirectory(workingDirectory_);

		connect(gitLogProcess_, SIGNAL(readyRead()), this, SLOT(onGitLogProcessReadReady()));
		connect(gitLogProcess_, SIGNAL(finished(int)), this, SLOT(onGitLogProcessFinished(int)));
		gitLogProcess_->start(program, arguments);
	}
	else{
		qDebug() << QString("Cannot continue with deployment, git branch exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
}

void AMDeploy::onGitLogProcessReadReady(){
	gitLogOutput_.append(gitLogProcess_->readAllStandardOutput());
//	qDebug() << "Git log says: " << gitLogOutput_;
}

void AMDeploy::onGitLogProcessFinished(int status){
//	qDebug() << "Git log exited with " << status;
	if(status == 0){
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

		gitDescribeProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "describe";
		gitDescribeProcess_->setWorkingDirectory(workingDirectory_);

		connect(gitDescribeProcess_, SIGNAL(readyRead()), this, SLOT(onGitDescribeProcessReadReady()));
		connect(gitDescribeProcess_, SIGNAL(finished(int)), this, SLOT(onGitDescribeProcessFinished(int)));
		gitDescribeProcess_->start(program, arguments);
	}
	else{
		qDebug() << QString("Cannot continue with deployment, git log exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
}

void AMDeploy::onGitDescribeProcessReadReady(){
	gitDescribeOutput_.append(gitDescribeProcess_->readAllStandardOutput());
//	qDebug() << "Git describe says: " << gitDescribeOutput_;
	currentGitDescription_ = gitDescribeOutput_.trimmed();
}

void AMDeploy::onGitDescribeProcessFinished(int status){
//	qDebug() << "Git describe exited with " << status;
	if(status == 0){
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

		qmakeVersionProcess_ = new QProcess();
		QString program = "qmake";
		QStringList arguments;
		arguments << "--version";
		qmakeVersionProcess_->setWorkingDirectory(workingDirectory_);

		connect(qmakeVersionProcess_, SIGNAL(readyRead()), this, SLOT(onQMakeVersionProcessReadReady()));
		connect(qmakeVersionProcess_, SIGNAL(finished(int)), this, SLOT(onQMakeVersionProcessFinished(int)));
		qmakeVersionProcess_->start(program, arguments);
	}
	else{
		qDebug() << QString("Cannot continue with deployment, git describe exited with status %1.").arg(status);
		QCoreApplication::instance()->exit(-1);
	}
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
		qmakeProcess_ = new QProcess();
		QString program = "qmake";
		QStringList arguments;
		arguments << projectFile_ << "-r";
		#ifdef Q_WS_MAC
		arguments << "-spec" << "unsupported/macx-clang" << "CONFIG+=x86_64";
		#endif
		arguments << "DEFINES+=AM_BUILD_REPORTER_ENABLED";
		qmakeProcess_->setWorkingDirectory(workingDirectory_);

		connect(qmakeProcess_, SIGNAL(readyRead()), this, SLOT(onQMakeProcessReadReady()));
		connect(qmakeProcess_, SIGNAL(finished(int)), this, SLOT(onQMakeProcessFinished(int)));
		qmakeProcess_->start(program, arguments);
	}
}

void AMDeploy::onQMakeProcessReadReady(){
	qDebug() << qmakeProcess_->readAll();
}

void AMDeploy::onQMakeProcessFinished(int status){
	if(status != 0){
		qDebug() << "The qmake process failed with an exit status of " << status;
		QCoreApplication::instance()->exit(-1);
	}
	else{
		makeProcess_ = new QProcess();
		QString program = "make";
		QStringList arguments;
		makeProcess_->setWorkingDirectory(workingDirectory_);

		connect(makeProcess_, SIGNAL(readyRead()), this, SLOT(onMakeProcessReadReady()));
		connect(makeProcess_, SIGNAL(finished(int)), this, SLOT(onMakeProcessFinished(int)));
		makeProcess_->start(program, arguments);
	}
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
