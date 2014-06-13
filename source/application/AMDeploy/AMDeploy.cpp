#include "AMDeploy.h"

#include <QProcess>
#include <QCoreApplication>

#include <QDebug>

AMDeploy::AMDeploy(QObject *parent) :
	QObject(parent)
{
	gitStatusProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "status" << "--short";
	gitStatusProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

	connect(gitStatusProcess_, SIGNAL(readyRead()), this, SLOT(onGitStatusProcessReadReady()));
	connect(gitStatusProcess_, SIGNAL(finished(int)), this, SLOT(onGitStatusProcessFinished(int)));
	gitStatusProcess_->start(program, arguments);
}

void AMDeploy::onGitStatusProcessReadReady(){
	gitStatusOutput_.append(gitStatusProcess_->readAllStandardOutput());
	qDebug() << "Git status says: " << gitStatusOutput_;
}

void AMDeploy::onGitStatusProcessFinished(int status){
	qDebug() << "Git status exited with " << status;
	if(status == 0){
		gitBranchProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "branch";
		gitBranchProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitBranchProcess_, SIGNAL(readyRead()), this, SLOT(onGitBranchProcessReadReady()));
		connect(gitBranchProcess_, SIGNAL(finished(int)), this, SLOT(onGitBranchProcessFinished(int)));
		gitBranchProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitBranchProcessReadReady(){
	gitBranchOutput_.append(gitBranchProcess_->readAllStandardOutput());
	qDebug() << "Git branch says: " << gitBranchOutput_;
}

void AMDeploy::onGitBranchProcessFinished(int status){
	qDebug() << "Git branch exited with " << status;
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
		gitLogProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitLogProcess_, SIGNAL(readyRead()), this, SLOT(onGitLogProcessReadReady()));
		connect(gitLogProcess_, SIGNAL(finished(int)), this, SLOT(onGitLogProcessFinished(int)));
		gitLogProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitLogProcessReadReady(){
	gitLogOutput_.append(gitLogProcess_->readAllStandardOutput());
	qDebug() << "Git log says: " << gitLogOutput_;
}

void AMDeploy::onGitLogProcessFinished(int status){
	qDebug() << "Git log exited with " << status;
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
		gitDescribeProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitDescribeProcess_, SIGNAL(readyRead()), this, SLOT(onGitDescribeProcessReadReady()));
		connect(gitDescribeProcess_, SIGNAL(finished(int)), this, SLOT(onGitDescribeProcessFinished(int)));
		gitDescribeProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitDescribeProcessReadReady(){
	gitDescribeOutput_.append(gitDescribeProcess_->readAllStandardOutput());
	qDebug() << "Git describe says: " << gitDescribeOutput_;
	currentGitDescription_ = gitDescribeOutput_;
}

void AMDeploy::onGitDescribeProcessFinished(int status){
	qDebug() << "Git describe exited with " << status;
	if(status == 0){
		qDebug() << "I think we've successfully done these steps";

		qDebug() << "Branch: " << currentBranch_;
		qDebug() << "Commit: " << currentCommitSHA_;
		qDebug() << "Author: " << currentCommitAuthor_;
		qDebug() << "Date: " << currentCommitDate_;
		qDebug() << "Describe: " << currentGitDescription_;

		QCoreApplication::instance()->quit();
	}
}

AMDeploy::~AMDeploy()
{
}
